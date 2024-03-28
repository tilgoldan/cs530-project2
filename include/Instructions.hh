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
};