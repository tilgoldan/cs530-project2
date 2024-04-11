#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class StatementType {
  DIRECTIVE = 0,
  LITERAL,
  FORMAT0,
  FORMAT1,
  FORMAT2,
  FORMAT3,
  FORMAT4,
  INVALID
};

struct AddressingMode {
  bool indirect;
  bool immediate;
  bool indexed;
  bool extended;
};

enum class LiteralFormat { CHAR, HEX, INVALID };

class Statement {
public:
  Statement(const std::string &line, int32_t location);
  ~Statement() = default;

  int32_t location() const { return m_location; }
  void set_location(int32_t location) { m_location = location; }

  StatementType type() const { return m_type; }
  const std::string &label() const { return m_label; }
  const std::string &opcode() const { return m_mnemonic; }
  const std::string &operand() const { return m_operand; }
  int32_t size() const { return m_size; }
  void set_size(int32_t size) { m_size = size; }

  bool has_label() const { return !m_label.empty(); }
  bool has_operand() const { return !m_operand.empty(); }

  void print_info() const;

  void assemble();

  void print_assembled(std::ostream &) const;

private:
  int32_t m_location;
  int32_t m_size;
  StatementType m_type;
  std::string m_label;
  std::string m_mnemonic;
  char m_mnemonic_prefix;
  std::string m_operand;

  AddressingMode m_addressing_mode;

  std::vector<uint8_t> m_assembled_bytes;

  std::string m_raw;

  StatementType find_type();
  void find_size();
  void find_addressing_mode();
  void parse_literal();
};