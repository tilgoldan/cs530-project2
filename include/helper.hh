/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
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

#define NOT_IMPLEMENTED(msg) assert(false && "Not yet implemented:" msg)
#define NOT_REACHED(msg) assert(false && "Not reached: " msg)