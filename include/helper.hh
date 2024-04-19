/*
Written-By: Til Goldan (131473651)
Written-By: Jasmine McDuffie (825467728)
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: helper.hh
*/

#pragma once

#include <string>

inline void trim(std::string &str) {
  str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
  str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}

inline bool is_number(const std::string &s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it))
    ++it;
  return !s.empty() && it == s.end();
}

std::vector<std::string> splitStringAlongDelimiters(const std::string &,
                                                    const std::string &);

std::vector<std::string> splitString(const std::string &input, char delimiter);

#define NOT_IMPLEMENTED(msg) assert(false && "Not yet implemented:" msg)
#define NOT_REACHED(msg) assert(false && "Not reached: " msg)