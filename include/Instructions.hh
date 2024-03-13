#pragma once

#include <map>

enum class InstructionFormat {
  FORMAT1 = 1,
  FORMAT2,
  FORMAT3_4
};

// type for function that handels a directive
struct InstructionInfo {
  int32_t opcode;
  InstructionFormat format;
  bool xe_exclusive;
};

class Instructions {
public:
  static std::map<std::string, InstructionInfo> &the();
};