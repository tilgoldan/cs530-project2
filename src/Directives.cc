/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Directives.cc
*/

#include "Directives.hh"
#include "Statement.hh"
#include "Symbol.hh"
#include "helper.hh"

#include <cassert>
#include <iostream>
#include <map>

void start(Statement &statement, ControlSection &section) {

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  if (section.location_counter() != -1) {
    std::cerr << "Location counter is already set to "
              << section.location_counter() << "\n";
    assert(false &&
           "START directive must be the first directive in the program");
  }

  const auto operand = statement.operand();
  int32_t location = std::stoi(operand, nullptr, 16);
  statement.set_location(location);
  section.set_location_counter(location);
  section.set_start_address(location);
  section.set_name(statement.label());
}

void end(Statement &statement, ControlSection &section) {

  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  if (section.location_counter() == -1) {
    std::cerr << "Location counter is not set\n";
    assert(false && "END directive must be after START directive");
  }

  section.process_pending_literals_next();

  section.set_end_address(section.location_counter());
}

void base(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::SECOND) {
    return;
  }

  section.set_base_relative_label(statement.operand());
}

void resw(Statement &statement, ControlSection &section) {
  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  const auto operand = statement.operand();
  int32_t size = std::stoi(operand, nullptr, 10);
  statement.set_size(size * 3);
}

void word(Statement &statement, ControlSection &section) {
  (void)section;

  if (section.current_pass() == Pass::FIRST) {
    statement.set_size(3);
    return;
  }

  const auto& op = statement.operand();

  if (op[1] == '\'') {
    // Format of "X'...'" or "C'...'""
    statement.parse_literal_data(statement.operand());
  } else {
    auto value = section.parse_value(op);
    statement.bytes_from_direct_value(value.value());
  }

  assert(statement.bytes().size() == 3 && "Invalid word directive");
}

void resb(Statement &statement, ControlSection &section) {
  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  const auto operand = statement.operand();
  int32_t size = std::stoi(operand, nullptr, 10);
  statement.set_size(size);
}

void byte(Statement &statement, ControlSection &section) {
  if (section.current_pass() == Pass::FIRST) {
    statement.set_size(1);
    return;
  }

  const auto &op = statement.operand();

  if (op[1] == '\'') {
    // Format of "X'...'" or "C'...'""
    statement.parse_literal_data(statement.operand());
  } else {
    auto value = section.parse_value(op);
    statement.bytes_from_direct_value(value.value());
  }

  assert(statement.bytes().size() == 1 && "Invalid byte directive");
}

void ltorg(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  section.process_pending_literals_next();
}

void equ(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  if (statement.operand() == "*") {
    // Set the label to the current location counter
    section.define_symbol(statement.label(), section.location_counter(),
                          SymbolType::RELATIVE);
  } else {
    auto info = section.parse_value(statement.operand());
    section.define_symbol(statement.label(), info.value(), info.type());
  }
}

void org(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  auto info = section.parse_value(statement.operand());
  section.set_location_counter(info.value());
}

void extref(Statement &statement, ControlSection &section) {
  (void)statement;

  for (const auto &symbol : splitString(statement.operand(), ',')) {
    section.define_symbol(symbol, 0, SymbolType::EXTERNAL);
  }
}

void extdef(Statement &statement, ControlSection &section) {
  (void)statement;
  (void)section;
  std::cerr << "TODO: extdef\n";
}

std::map<std::string, DirectiveHandler> &Directives::the() {
  static std::map<std::string, DirectiveHandler> s_directives = {
      {"START", start}, {"END", end},   {"BASE", base},     {"RESW", resw},
      {"RESB", resb},   {"BYTE", byte}, {"WORD", word},     {"ORG", org},
      {"LTORG", ltorg}, {"EQU", equ},   {"EXTREF", extref}, {"EXTDEF", extdef}};
  return s_directives;
}