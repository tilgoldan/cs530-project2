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
  (void)section;

  const auto operand = statement.operand();
  int32_t size = std::stoi(operand, nullptr, 10);
  statement.set_size(size * 3);
}

std::map<std::string, DirectiveHandler> &Directives::the() {
  static std::map<std::string, DirectiveHandler> s_directives = {
      {"START", dir_start},
      {"END", dir_end},
      {"BASE", dir_base},
      {"RESW", dir_resw},
  };
  return s_directives;
}