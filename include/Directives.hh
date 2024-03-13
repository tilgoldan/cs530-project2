#pragma once

#include <map>

#include "ControlSection.hh"
#include "Statement.hh"

// type for function that handels a directive
typedef void (*DirectiveHandler)(Statement& statement, ControlSection &section);

class Directives {
public:
  static std::map<std::string, DirectiveHandler> &the();

};