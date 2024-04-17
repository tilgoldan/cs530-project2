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

void parse_value(std::string s) {
  // value can be: constant, other symbol, or an expression
  //NOT_IMPLEMENTED("parse_value");
}

void dir_start(Statement &statement, ControlSection &section) {

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

void dir_end(Statement &statement, ControlSection &section) {

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

void dir_base(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::SECOND) {
    return;
  }

  section.set_base_relative_label(statement.operand());
}

void dir_resw(Statement &statement, ControlSection &section) {
  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  const auto operand = statement.operand();
  int32_t size = std::stoi(operand, nullptr, 10);
  statement.set_size(size * 3);
}

void dir_word(Statement &statement, ControlSection &section) {
  (void)section;

  if (section.current_pass() == Pass::FIRST) {
    statement.set_size(3);
    return;
  }

  NOT_IMPLEMENTED("word directive 2nd pass");
}

void dir_resb(Statement &statement, ControlSection &section) {
  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  const auto operand = statement.operand();
  int32_t size = std::stoi(operand, nullptr, 10);
  statement.set_size(size);
}

void dir_byte(Statement &statement, ControlSection &section) {
  if (section.current_pass() == Pass::FIRST) {
    statement.set_size(1);
    return;
  }

  // Second pass
  // NOT_IMPLEMENTED("byte directive 2nd pass");

  // const auto operand = statement.operand();
  // if (operand[0] == 'C') {
  //   statement.set_size(operand.size() - 3);
  // } else if (operand[0] == 'X') {
  //   statement.set_size((operand.size() - 3) / 2);
  // } else {
  //   std::cerr << "Invalid BYTE operand: " << operand << "\n";
  //   assert(false && "Invalid BYTE operand");
  // }
}

void dir_ltorg(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  section.process_pending_literals_next();
}

void dir_equ(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  // Expressions can be classified as absolute
  // expressions or relative expressions MAXLEN EQU BUFEND - BUFFER
// – BUFEND and BUFFER both are relative terms,
//       representing addresses within the program
// – However the expression BUFEND - BUFFER represents an absolute value
// • When relative terms are paired with opposite signs,
//       the dependency on the program starting addres

              std::cerr
          << "TODO: EQU\n";
  parse_value("");

  if (statement.operand() == "*") {
    // Set the label to the current location counter
    //section.define_label(statement.label(), section.location_counter());
  } else {
    // Evaluate the expression
    // TODO
    section.define_symbol(statement.label(), 0xdeadc0de, Symbol::RELATIVE);
  }
}

void dir_org(Statement &statement, ControlSection &section) {
  (void)statement;

  if (section.current_pass() != Pass::FIRST) {
    return;
  }

  // – indirectly assign values to symbols
  // – reset the location counter to the specified value
  // – Usage : ORG value

  std::cerr << "TODO: ORG\n";
  parse_value("");
}

std::map<std::string, DirectiveHandler> &Directives::the() {
  static std::map<std::string, DirectiveHandler> s_directives = {
      {"START", dir_start}, {"END", dir_end},   {"BASE", dir_base},
      {"RESW", dir_resw},   {"RESB", dir_resb}, {"BYTE", dir_byte},
      {"WORD", dir_word},   {"ORG", dir_org},   {"LTORG", dir_ltorg},
      {"EQU", dir_equ},
  };
  return s_directives;
}