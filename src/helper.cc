/*
Written-By: Til Goldan (131473651)
Written-By: Jasmine McDuffie (825467728)
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: helper.cc
*/

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "helper.hh"

std::vector<std::string>
splitStringAlongDelimiters(const std::string &input,
                           const std::string &delimiters) {
  std::vector<std::string> result;
  std::string current;

  for (char c : input) {
    if (std::find(delimiters.begin(), delimiters.end(), c) !=
        delimiters.end()) {
      if (!current.empty()) {
        result.push_back(current);
        current.clear();
      }
      result.push_back(std::string(1, c));
    } else {
      current += c;
    }
  }

  if (!current.empty()) {
    result.push_back(current);
  }

  return result;
}

std::vector<std::string> splitString(const std::string &input, char delimiter) {
  std::vector<std::string> result;
  std::stringstream ss(input);
  std::string item;

  while (std::getline(ss, item, delimiter)) {
    result.push_back(item);
  }

  return result;
}