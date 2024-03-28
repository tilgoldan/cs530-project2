
#include <cassert>
#include <iomanip>
#include <ios>
#include <iostream>
#include <ostream>

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

void Statement::parse_literal() {
  assert(m_label == "*");

  auto data = m_mnemonic;

  // Starting '='
  assert(data[0] == '=');
  data = data.substr(1);

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
        << m_location;
  }

  out << "    ";

  // Label
  out << std::setw(7) << std::setfill(' ') << std::left << m_label;

  out << " ";

  // Mnemonic
  auto prefix = m_mnemonic[0];
  int width = 8;
  if (prefix != '+' && prefix != '=') {
    out << " ";
    width--;
  }
  out << std::setw(width) << std::setfill(' ') << std::left << m_mnemonic;

  out << " ";

  // Operand
  prefix = m_operand[0];
  width = 26;
  if (prefix != '#' && prefix != '=') {
    out << " ";
    width--;
  }
  out << std::setw(width) << std::setfill(' ') << std::left << m_operand;

  for (auto byte : m_assembled_bytes) {
    out << std::setw(2) << std::setfill('0') << std::hex
        << std::uppercase << static_cast<int>(byte);
  }
  
  out << "\n";
}