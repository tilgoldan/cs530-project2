#include "Directives.hh"
#include "Statement.hh"

#include <cassert>
#include <iostream>
#include <map>

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
}

void dir_base(Statement &statement, ControlSection &section) {

  if (section.current_pass() != Pass::SECOND) {
    return;
  }

  assert(false && "BASE directive is not supported");
}

void dir_resw(Statement &statement, ControlSection &section) {
  const auto operand = statement.operand();
  int32_t size = std::stoi(operand, nullptr, 10);
  statement.set_size(size * 3);
}

std::map<std::string, DirectiveHandler> &Directives::the() {
  static std::map<std::string, DirectiveHandler> s_directives = {
    {"START", dir_start},
    {"BASE", dir_base},
    // {"RESW", dir_resw},
  };
  return s_directives;
}