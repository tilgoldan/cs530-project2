/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Instructions.hh
*/

#pragma once

#include <map>

enum class OperantType {
  NONE,
  REGISTERS,
  MEMORY,
};

// type for function that handels a directive
struct InstructionInfo {
  int32_t opcode;
  OperantType op_type;
  bool xe_exclusive;
};

class Instructions {
public:
  static std::map<std::string, InstructionInfo> &the();
  static std::map<std::string, int32_t> &regs();
};