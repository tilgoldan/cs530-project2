/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Instructions.cc
*/

#include <cassert>
#include <map>

#include "Instructions.hh"

std::map<std::string, int32_t> &Instructions::regs() {
  static std::map<std::string, int32_t> s_regs = {
      {"A", 0}, {"X", 1}, {"L", 2}, {"B", 3}, {"S", 4}, {"T", 5}, {"F", 6},
      {"PC", 8}, {"SW", 9},
  };
  return s_regs;
}

std::map<std::string, InstructionInfo> &Instructions::the() {
  static std::map<std::string, InstructionInfo> s_instructions = {
      {"ADD", {0x18, OperantType::MEMORY, false}},
      {"ADDF", {0x58, OperantType::MEMORY, true}},
      {"ADDR", {0x90, OperantType::REGISTERS, true}},
      {"AND", {0x40, OperantType::MEMORY, false}},
      {"CLEAR", {0xB4, OperantType::REGISTERS, true}},
      {"COMP", {0x28, OperantType::MEMORY, false}},
      {"COMPF", {0x88, OperantType::MEMORY, true}},
      {"COMPR", {0xA0, OperantType::REGISTERS, true}},
      {"DIV", {0x24, OperantType::MEMORY, false}},
      {"DIVF", {0x64, OperantType::MEMORY, true}},
      {"DIVR", {0x9C, OperantType::REGISTERS, true}},
      {"FIX", {0xC4, OperantType::NONE, true}},
      {"FLOAT", {0xC0, OperantType::NONE, true}},
      {"HIO", {0xF4, OperantType::NONE, true}},
      {"J", {0x3C, OperantType::MEMORY, false}},
      {"JEQ", {0x30, OperantType::MEMORY, false}},
      {"JGT", {0x34, OperantType::MEMORY, false}},
      {"JLT", {0x38, OperantType::MEMORY, false}},
      {"JSUB", {0x48, OperantType::MEMORY, false}},
      {"LDA", {0x00, OperantType::MEMORY, false}},
      {"LDB", {0x68, OperantType::MEMORY, true}},
      {"LDCH", {0x50, OperantType::MEMORY, false}},
      {"LDF", {0x70, OperantType::MEMORY, true}},
      {"LDL", {0x08, OperantType::MEMORY, false}},
      {"LDS", {0x6C, OperantType::MEMORY, true}},
      {"LDT", {0x74, OperantType::MEMORY, true}},
      {"LDX", {0x04, OperantType::MEMORY, false}},
      {"LPS", {0xD0, OperantType::MEMORY, true}},
      {"MUL", {0x20, OperantType::MEMORY, false}},
      {"MULF", {0x60, OperantType::MEMORY, true}},
      {"MULR", {0x98, OperantType::REGISTERS, true}},
      {"NORM", {0xC8, OperantType::NONE, true}},
      {"OR", {0x44, OperantType::MEMORY, false}},
      {"RD", {0xD8, OperantType::MEMORY, false}},
      {"RMO", {0xAC, OperantType::REGISTERS, true}},
      {"RSUB", {0x4C, OperantType::MEMORY, false}},
      {"SHIFTL", {0xA4, OperantType::REGISTERS, true}},
      {"SHIFTR", {0xA8, OperantType::REGISTERS, true}},
      {"SIO", {0xF0, OperantType::NONE, true}},
      {"SSK", {0xEC, OperantType::MEMORY, true}},
      {"STA", {0x0C, OperantType::MEMORY, false}},
      {"STB", {0x78, OperantType::MEMORY, true}},
      {"STCH", {0x54, OperantType::MEMORY, false}},
      {"STF", {0x80, OperantType::MEMORY, true}},
      {"STI", {0xD4, OperantType::MEMORY, true}},
      {"STL", {0x14, OperantType::MEMORY, false}},
      {"STS", {0x7C, OperantType::MEMORY, true}},
      {"STSW", {0xE8, OperantType::MEMORY, false}},
      {"STT", {0x84, OperantType::MEMORY, true}},
      {"STX", {0x10, OperantType::MEMORY, false}},
      {"SUB", {0x1C, OperantType::MEMORY, false}},
      {"SUBF", {0x5C, OperantType::MEMORY, true}},
      {"SUBR", {0x94, OperantType::REGISTERS, true}},
      {"SVC", {0xB0, OperantType::REGISTERS, true}},
      {"TD", {0xE0, OperantType::MEMORY, false}},
      {"TIO", {0xF8, OperantType::NONE, true}},
      {"TIX", {0x2C, OperantType::MEMORY, false}},
      {"TIXR", {0xB8, OperantType::REGISTERS, true}},
      {"WD", {0xDC, OperantType::MEMORY, false}},
  };
  return s_instructions;
}