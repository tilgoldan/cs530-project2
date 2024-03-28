/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: main.hh
*/

#pragma once

#include <string>

inline void trim(std::string &str) {
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
}

#define NOT_IMPLEMENTED(msg) assert(false && "Not yet implemented:" msg)
#define NOT_REACHED(msg) assert(false && "Not reached: " msg)