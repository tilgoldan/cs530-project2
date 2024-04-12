/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Directives.hh
*/

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