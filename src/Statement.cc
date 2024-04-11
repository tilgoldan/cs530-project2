
#include <cassert>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>
#include <system_error>

#include "Directives.hh"
#include "Instructions.hh"
#include "Statement.hh"
#include "helper.hh"

Statement::Statement(const std::string &line, int32_t location)
    : m_location(location), m_size(0) {
  m_type = StatementType::INVALID;
  m_raw = line;
  m_label = line.substr(0, 6);
  m_mnemonic = line.substr(6, 11);

  size_t length = line.size();

  if (length > 17) {
    m_operand = line.substr(17, length - 17);
  }

  trim(m_label);
  trim(m_mnemonic);
  trim(m_operand);

  if (m_mnemonic[0] == '+' || m_mnemonic[0] == '=') {
    m_mnemonic_prefix = m_mnemonic[0];
    m_mnemonic = m_mnemonic.substr(1);
  } else {
    m_mnemonic_prefix = ' ';
  }

  m_type = find_type();

  if (m_type == StatementType::LITERAL) {
    parse_literal();
  }

  find_size();

  assert(!m_mnemonic.empty());
}

StatementType Statement::find_type() {
  auto directives = Directives::the();
  auto instructions = Instructions::the();

  auto mnemonic = m_mnemonic;

  if (m_label == "*") {
    return StatementType::LITERAL;
  }

  if (directives.find(mnemonic) != directives.end()) {
    return StatementType::DIRECTIVE;
  } else if (instructions.find(mnemonic) != instructions.end()) {
    auto instr = instructions[mnemonic];
    OperantType op_type = instr.op_type;
    find_addressing_mode();

    std::cerr << "Instruction: " << mnemonic
              << " Operant Type: " << static_cast<int>(op_type) << "\n";

    switch (op_type) {
    case OperantType::NONE:
      return StatementType::FORMAT1;
    case OperantType::REGISTERS:
      return StatementType::FORMAT2;
    case OperantType::MEMORY:
      if (m_addressing_mode.extended) {
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

void Statement::parse_literal() {
  assert(m_label == "*");

  // Starting '='
  assert(m_mnemonic_prefix == '=');
  auto data = m_mnemonic;

  // Format
  LiteralFormat format;
  switch (data[0]) {
  case 'C':
    format = LiteralFormat::CHAR;
    break;
  case 'X':
    format = LiteralFormat::HEX;
    break;
  default:
    NOT_REACHED("Invalid literal format");
  }
  data = data.substr(1);

  // Surrounding quotes
  assert(data[0] == '\'' && data[data.size() - 1] == '\'');
  data = data.substr(1, data.size() - 2);

  switch (format) {
  case LiteralFormat::CHAR:
    for (size_t i = 0; i < data.size(); i++) {
      m_assembled_bytes.push_back(data[i]);
    }
    break;
  case LiteralFormat::HEX:
    assert(data.size() % 2 == 0 && "Invalid hex literal");
    for (size_t i = 0; i < data.size(); i += 2) {
      std::string byte = data.substr(i, 2);
      m_assembled_bytes.push_back(std::stoi(byte, nullptr, 16));
    }
    break;
  case LiteralFormat::INVALID:
    NOT_REACHED("Invalid literal format");
  }
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
  case StatementType::LITERAL:
    size = m_assembled_bytes.size();
    break;
  default:
    break;
  }

  m_size = size;
}

void Statement::assemble() {
  auto instructions = Instructions::the();
  auto directives = Directives::the();

  switch (m_type) {
  case StatementType::LITERAL:
  case StatementType::DIRECTIVE: {
    return;
  }
  case StatementType::FORMAT0: {
    assert(false && "SIC Basic Instruction Format is not supported");
  }
  case StatementType::INVALID: {
    assert(false && "Invalid statement");
  }
  default:
    break;
  }

  auto instr = instructions.at(m_mnemonic);
  auto opcode = instr.opcode;

  std::cerr << "Instruction: " << m_mnemonic << " Opcode: " << opcode << "\n";
  std::cerr << "Addressing Mode: "
            << "Indirect: " << m_addressing_mode.indirect
            << " Immediate: " << m_addressing_mode.immediate
            << " Indexed: " << m_addressing_mode.indexed
            << " Extended: " << m_addressing_mode.extended << "\n";

  if (m_addressing_mode.indirect) {
    opcode |= 0x02;
  } else if (m_addressing_mode.immediate) {
    opcode |= 0x01;
  } else {
    opcode |= 0x03;
  }

  m_assembled_bytes.push_back(opcode & 0xFF);
}

void Statement::find_addressing_mode() {
  if (m_operand.empty()) {
    return;
  }

  m_addressing_mode.immediate = (m_operand[0] == '#');
  m_addressing_mode.indirect = (m_operand[0] == '@');
  m_addressing_mode.indexed = (m_operand.find(",X") != std::string::npos);
  m_addressing_mode.extended = (m_mnemonic_prefix == '+');
}

void Statement::print_info() const {
  std::cerr << "Statement(\n";
  std::cerr << "  loc: " << m_location << "\n";
  std::cerr << "  label: '" << m_label << "'\n";
  std::cerr << "  mnemonic: '" << m_mnemonic << "'\n";
  std::cerr << "  operand: '" << m_operand << "'\n";
  std::cerr << "  type: " << static_cast<int>(m_type) << "\n";
  std::cerr << ")\n";
}

void Statement::print_assembled(std::ostream &out) const {

  // Location
  if (m_mnemonic == "END") {
    out << "    ";
  } else {
    out << std::hex << std::uppercase << std::setw(4) << std::setfill('0')
        << std::right << m_location;
  }

  out << "    ";

  // Label
  out << std::setw(7) << std::setfill(' ') << std::left << m_label;

  out << " ";

  // Mnemonic

  out << m_mnemonic_prefix;
  
  int width = 8;
  out << std::setw(width) << std::setfill(' ') << std::left
      << m_mnemonic;

  out << " ";

  // Operand
  char prefix = m_operand[0];
  width = 26;
  if (prefix != '#' && prefix != '=') {
    out << " ";
    width--;
  }
  out << std::setw(width) << std::setfill(' ') << std::left << m_operand;

  for (auto byte : m_assembled_bytes) {
    out << std::setw(2) << std::setfill('0') << std::hex << std::right
        << std::uppercase << static_cast<int>(byte);
  }
  
  out << "\n";
}