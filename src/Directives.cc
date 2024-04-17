/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Directives.cc
*/

#include "Directives.hh"
#include "Symbol.hh"
#include "Statement.hh"
#include "helper.hh"

#include <cassert>
#include <iostream>
#include <map>

SymbolInfo parse_value(std::string s) {
  // value can be: constant, other symbol, or an expression
  // NOT_IMPLEMENTED("parse_value");

  // Expressions can be classified as absolute
  // expressions or relative expressions MAXLEN EQU BUFEND - BUFFER
  // – BUFEND and BUFFER both are relative terms,
  //       representing addresses within the program
  // – However the expression BUFEND - BUFFER represents an absolute value
  // • When relative terms are paired with opposite signs,
  //       the dependency on the program starting addres

  std::cerr << "TODO: parse_value\n";

  return SymbolInfo(0xdeadc0de, Symbol::ABSOLUTE);
}

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

  statement.parse_literal_data(statement.operand());

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

  statement.parse_literal_data(statement.operand());

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
                          Symbol::RELATIVE);
  } else {
    auto info = parse_value(statement.operand());
    section.define_symbol(statement.label(), info.address(), info.type());
  }
}

void org(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  auto info = parse_value(statement.operand());
  section.set_location_counter(info.address());
}

std::map<std::string, DirectiveHandler> &Directives::the() {
  static std::map<std::string, DirectiveHandler> s_directives = {
      {"START", start}, {"END", end},   {"BASE", base},
      {"RESW", resw},   {"RESB", resb}, {"BYTE", byte},
      {"WORD", word},   {"ORG", org},   {"LTORG", ltorg},
      {"EQU", equ},
  };
  return s_directives;
}