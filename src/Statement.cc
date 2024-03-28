
#include <cassert>
#include <iostream>

#include "Directives.hh"
#include "Instructions.hh"
#include "Statement.hh"
#include "helper.hh"

Statement::Statement(const std::string &line, int32_t location)
    : m_location(location), m_size(0) {
  m_type = StatementType::INVALID;
  m_label = line.substr(0, 6);
  m_mnemonic = line.substr(6, 11);

  size_t length = line.size();

  if (length > 17) {
    m_operand = line.substr(17, length - 17);
  }

  trim(m_label);
  trim(m_mnemonic);
  trim(m_operand);

  m_type = find_type();
  find_size();

  assert(!m_mnemonic.empty());
}

StatementType Statement::find_type() {
  auto directives = Directives::the();
  auto instructions = Instructions::the();

  auto mnemonic = m_mnemonic;
  if (mnemonic[0] == '+') {
    mnemonic = mnemonic.substr(1);
  }

  if (directives.find(mnemonic) != directives.end()) {
    return StatementType::DIRECTIVE;
  } else if (instructions.find(mnemonic) != instructions.end()) {
    auto instr = instructions[mnemonic];
    OperantType op_type = instr.op_type;
    AddressingMode mode = find_addressing_mode();

    std::cerr << "Instruction: " << mnemonic
              << " Operant Type: " << static_cast<int>(op_type) << "\n";

    switch (op_type) {
    case OperantType::NONE:
      return StatementType::FORMAT1;
    case OperantType::REGISTERS:
      return StatementType::FORMAT2;
    case OperantType::MEMORY:
      if (mode.extended) {
        return StatementType::FORMAT4;
      } else {
        return StatementType::FORMAT3;
      }
    default:
      NOT_REACHED();
    }
  }

  std::cerr << "Invalid statement: " << mnemonic << "\n";
  return StatementType::INVALID;
}

void Statement::find_size() {
  int32_t size = 0;

  switch (m_type) {
  case StatementType::FORMAT0:
    size = 3;
    break;
  case StatementType::FORMAT1:
    size = 1;
    break;
  case StatementType::FORMAT2:
    size = 2;
    break;
  case StatementType::FORMAT3:
    size = 3;
    break;
  case StatementType::FORMAT4:
    size = 4;
    break;
  default:
    break;
  }

  m_size = size;
}

AddressingMode Statement::find_addressing_mode() {
  AddressingMode mode;

  if (m_operand.empty()) {
    return mode;
  }

  mode.immediate = (m_operand[0] == '#');
  mode.indirect = (m_operand[0] == '@');
  mode.indexed = (m_operand.find(",X") != std::string::npos);
  mode.extended = (m_mnemonic[0] == '+');

  return mode;
}

void Statement::print() const {
  std::cerr << "Statement(\n";
  std::cerr << "  loc: " << m_location << "\n";
  std::cerr << "  label: '" << m_label << "'\n";
  std::cerr << "  mnemonic: '" << m_mnemonic << "'\n";
  std::cerr << "  operand: '" << m_operand << "'\n";
  std::cerr << "  type: " << static_cast<int>(m_type) << "\n";
  std::cerr << ")\n";
}