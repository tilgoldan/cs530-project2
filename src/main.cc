/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: main.cc
*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "ControlSection.hh"

int main(int argc, char **argv) {
  std::vector<std::shared_ptr<ControlSection>> sections;

  for (int i = 1; i < argc; i++) {
    sections.push_back(std::make_shared<ControlSection>(argv[i]));
  }

  for (auto &section : sections) {
    section->first_pass();
  }

  for (auto &section : sections) {
    section->second_pass();
  }

  for (auto &section : sections) {
    section->print_listing(std::cout);
    section->print_tables(std::cout);
  }

  return 0;
}
