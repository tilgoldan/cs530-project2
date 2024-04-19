/*
Written-By: Til Goldan (131473651)
Written-By: Jasmine McDuffie (825467728)
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: main.cc
*/

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ControlSection.hh"

#ifndef PROGRAM_NAME
#define PROGRAM_NAME "NOT_SET"
#endif

void usage() {
  std::cerr << "Assemble SIC/XE assembly code into object code.\n";
  std::cerr << "Usage: " PROGRAM_NAME " <file> [<file> ...]\n";
}

int main(int argc, char **argv) {
  std::vector<std::shared_ptr<ControlSection>> sections;

  if (argc < 2) {
    usage();
    return 1;
  }

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
    auto file_base = section->path();
    file_base = file_base.substr(0, file_base.size() - 4);

    std::ofstream o_listing(file_base + ".l");
    section->print_object_code(o_listing);
    std::cout << "Wrote object code: " << file_base << ".l\n";

    std::ofstream o_tables(file_base + ".st");
    section->print_tables(o_tables);
    std::cout << "Wrote symbol table: " << file_base << ".st\n";
  }

  return 0;
}
