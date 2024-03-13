#include <cassert>
#include <map>

#include "Instructions.hh"

std::map<std::string, InstructionInfo> &Instructions::the(){
  static std::map<std::string, InstructionInfo> s_instructions = {
    {"ADD", {0x18, InstructionFormat::FORMAT3_4, false}},
    {"ADDF", {0x58, InstructionFormat::FORMAT3_4, true}},
    {"ADDR", {0x90, InstructionFormat::FORMAT2, false}},
    {"AND", {0x40, InstructionFormat::FORMAT3_4, false}},
    {"CLEAR", {0xB4, InstructionFormat::FORMAT2, false}},
    {"COMP", {0x28, InstructionFormat::FORMAT3_4, false}},
    {"COMPF", {0x88, InstructionFormat::FORMAT3_4, true}},
    {"COMPR", {0xA0, InstructionFormat::FORMAT2, false}},
    {"DIV", {0x24, InstructionFormat::FORMAT3_4, false}},
    {"DIVF", {0x64, InstructionFormat::FORMAT3_4, true}},
    {"DIVR", {0x9C, InstructionFormat::FORMAT2, false}},
    {"FIX", {0xC4, InstructionFormat::FORMAT1, false}},
    {"FLOAT", {0xC0, InstructionFormat::FORMAT1, false}},
    {"HIO", {0xF4, InstructionFormat::FORMAT1, false}},
    {"J", {0x3C, InstructionFormat::FORMAT3_4, false}},
    {"JEQ", {0x30, InstructionFormat::FORMAT3_4, false}},
    {"JGT", {0x34, InstructionFormat::FORMAT3_4, false}},
    {"JLT", {0x38, InstructionFormat::FORMAT3_4, false}},
    {"JSUB", {0x48, InstructionFormat::FORMAT3_4, false}},
    {"LDA", {0x00, InstructionFormat::FORMAT3_4, false}},
    {"LDB", {0x68, InstructionFormat::FORMAT3_4, false}},
    {"LDCH", {0x50, InstructionFormat::FORMAT3_4, false}},
    {"LDF", {0x70, InstructionFormat::FORMAT3_4, true}},
    {"LDL", {0x08, InstructionFormat::FORMAT3_4, false}},
    {"LDS", {0x6C, InstructionFormat::FORMAT3_4, false}},
    {"LDT", {0x74, InstructionFormat::FORMAT3_4, false}},
    {"LDX", {0x04, InstructionFormat::FORMAT3_4, false}}
    };
return s_instructions;
}