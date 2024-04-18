/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: ControlSection.cc
*/

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "ControlSection.hh"
#include "Directives.hh"
#include "Literal.hh"
#include "Statement.hh"
#include "Symbol.hh"
#include "helper.hh"

ControlSection::ControlSection(std::string path)
    : m_path(path), m_location_counter(-1), m_start_address(-1),
      m_end_address(-1), m_current_pass(Pass::NONE) {

  // Check path ends with '.sic'
  if (path.size() < 4 || path.substr(path.size() - 4) != ".sic") {
    assembler_error("Invalid file extension");
  }

  // Read the file at path into m_raw
  std::ifstream i_file(path);

  std::basic_ostringstream<char> os;
  os << i_file.rdbuf();
  m_raw = os.str();

  std::cerr << "Read " << m_raw.size() << " bytes from " << path << std::endl;
}

void ControlSection::define_pending_literals() {
  for (auto &it : m_literals) {
    if (it.second->address() != -1) {
      // Already defined
      continue;
    }

    std::stringstream ss;

    ss << "*        =";
    ss << it.first;

    std::string line = ss.str();

    Statement stmt = Statement(line, m_location_counter, -1);
    m_location_counter += stmt.size();
    it.second->set_address(stmt.location());
    it.second->set_bytes(stmt.bytes());
    m_statements.push_back(stmt);
  }
}

std::shared_ptr<SymbolInfo>
ControlSection::define_symbol(const std::string name, int32_t address,
                              SymbolType type) {
  return m_symbols[name] = std::make_shared<SymbolInfo>(address, type);
}

std::shared_ptr<SymbolInfo>
ControlSection::get_symbol(const std::string name) const {
  if (m_symbols.find(name) != m_symbols.end()) {
    return m_symbols.at(name);
  }

  return nullptr;
}

std::shared_ptr<LiteralInfo>
ControlSection::define_literal(const std::string name, int32_t address) {
  return m_literals[name] = std::make_shared<LiteralInfo>(address);
}

std::shared_ptr<LiteralInfo>
ControlSection::define_pending_literal(const std::string name) {
  return m_literals[name] = std::make_shared<LiteralInfo>();
}

std::shared_ptr<LiteralInfo>
ControlSection::get_literal(const std::string name) const {
  if (m_literals.find(name) != m_literals.end()) {
    return m_literals.at(name);
  }

  return nullptr;
}

SymbolInfo ControlSection::parse_value(const std::string &s) const {
  enum Op {
    ADD,
    SUB,
  };

  auto vec = splitStringAlongDelimiters(s, "+-");

  bool negative = false;
  bool next_token_is_op = false;
  Op current_op = ADD;

  SymbolType final_type = SymbolType::ABSOLUTE;

  int32_t value = 0;

  for (size_t i = 0; i < vec.size(); i++) {
    if (i == 0 && vec[i] == "-") {
      negative = true;
      continue;
    }

    if (next_token_is_op) {
      switch (vec[i][0]) {
      case '+':
        current_op = ADD;
        break;
      case '-':
        current_op = SUB;
        break;
      default:
        NOT_REACHED("Invalid operator");
      }
    } else {
      int32_t acu = 0;
      SymbolType type = SymbolType::ABSOLUTE;

      if (is_number(vec[i])) {
        acu = std::stoi(vec[i], nullptr, 10);
      } else {
        auto symbol = get_symbol(vec[i]);
        if (!symbol) {
          NOT_REACHED("Symbol not found");
        }

        acu = symbol->value();
        type = symbol->type();

        if (type == SymbolType::EXTERNAL) {
          // Not sure if this is correct
          type = SymbolType::RELATIVE;
        }
      }

      switch (current_op) {
      case ADD:
        value += acu;

        if (type == SymbolType::RELATIVE) {
          final_type = SymbolType::RELATIVE;
        }
        break;
      case SUB:
        value -= acu;

        if (type == SymbolType::RELATIVE) {
          final_type = SymbolType::ABSOLUTE;
        }
        break;
      }
    }
    next_token_is_op = !next_token_is_op;
  }

  if (negative) {
    value = -value;
  }

  return SymbolInfo(value, final_type);
}

// Pass 1 (define symbols) :
// 1. Assign addresses to all statements in the program.
// 2. Save the values(addresses) assigned to all labels for use in Pass .2
// 3. Perform some processing of assembler directives. (This includes processing
// that affects address assignment, such as determining the length of data areas
// defined by BYTE, RESW, etc.)

// Symbol length = 6 Byte

void ControlSection::first_pass() {
  m_current_pass = Pass::FIRST;

  std::istringstream i_stream(m_raw);

  if (!i_stream.good()) {
    assembler_error("Failed to open file");
  }

  std::string line;

  auto &directives = Directives::the();

  int32_t line_number = 0;

  while (std::getline(i_stream, line)) {

    line_number++;

    // Skip empty lines
    if (line.empty()) {
      continue;
    }

    // Skip comments
    if (line[0] == '.') {
      continue;
    }

    Statement stmt = Statement(line, m_location_counter, line_number);

    if (stmt.type() == StatementType::DIRECTIVE) {
      // Handle directives
      auto handler = directives[stmt.opcode()];
      // This might modify the statement and the control section
      handler(stmt, *this);
    } else if (stmt.type() == StatementType::INVALID) {
      assembler_error("Invalid statement", stmt);
    }

    // Make sure the location counter was set by START directive
    if (m_location_counter == -1) {
      assembler_error("Program does not start with START directive", stmt);
    }

    if (stmt.has_label() && stmt.type() != StatementType::LITERAL &&
        stmt.opcode() != "EQU") {

      if (get_symbol(stmt.label())) {
        assembler_error("Duplicate symbol", stmt);
      }

      define_symbol(stmt.label(), m_location_counter, SymbolType::RELATIVE);

    } else if (stmt.type() == StatementType::LITERAL) {
      // Save the value of the literal
      auto literal = define_literal(stmt.opcode(), stmt.location());
      literal->set_bytes(stmt.bytes());
    }

    // Handle literals in the operand field
    if (stmt.operand_prefix() == '=' && !get_literal(stmt.operand())) {
      // Save the literal for later
      define_pending_literal(stmt.operand());
    }

    m_statements.push_back(stmt);
    m_location_counter += stmt.size();

    if (m_process_pending_literals) {
      define_pending_literals();
      m_process_pending_literals = false;
    }
  }

  if (m_statements.size() == 0) {
    assembler_error("No statements found");
  }

  std::cout << "Pass 1 complete for " << m_path << std::endl;
}

// Pass 2 (assemble instructions and generate object program):
// 1. Assemble instructions (translating operation codes and looking up
// addresses).
// 2. Generate data values defined by BYTE, WORD, etc.
// 3. Perform processing of assembler directives not done during Pass 1.
// 4. Write the object program and the assembly listing.

void ControlSection::second_pass() {
  m_current_pass = Pass::SECOND;

  for (auto &stmt : m_statements) {
    if (stmt.type() == StatementType::DIRECTIVE) {
      auto &directives = Directives::the();
      auto handler = directives[stmt.opcode()];
      handler(stmt, *this);
      continue;
    } else if (stmt.type() == StatementType::INVALID) {
      assembler_error("Invalid statement", stmt);
    }

    // Assemble instructions

    if (stmt.type() == StatementType::LITERAL) {
      // Done in first pass
    } else {
      // Normal instruction
      // Assemble the statement
      stmt.assemble(*this);
    }
  }

  std::cout << "Pass 2 complete for " << m_path << std::endl;
}

void ControlSection::print_object_code(std::ostream &os) const {
  for (const auto &stmt : m_statements) {
    stmt.print_assembled(os);
  }
}

void ControlSection::print_tables(std::ostream &os) const {

  const auto print_csect = [](std::ostream &os, const std::string &name,
                              int32_t start, int32_t length) {
    os << std::setw(7) << std::setfill(' ') << std::left << name;
    os << "         ";
    os << std::setw(6) << std::setfill('0') << std::right << std::hex << start;
    os << "  ";
    os << std::setw(6) << std::setfill('0') << std::right << std::hex << length;
    os << "\n";
  };

  const auto print_symbol = [](std::ostream &os, const std::string &name,
                               const std::shared_ptr<SymbolInfo> &info) {
    os << "        ";
    os << std::setw(7) << std::setfill(' ') << std::left << name;
    os << " ";
    if (info->type() == SymbolType::EXTERNAL) {
      os << std::setw(6) << std::setfill(' ') << std::left << std::hex << "";
    } else {
      os << std::setw(6) << std::setfill('0') << std::right << std::hex
         << info->value();
    }
    os << "          ";
    switch (info->type()) {
    case SymbolType::ABSOLUTE:
      os << "A";
      break;
    case SymbolType::RELATIVE:
      os << "R";
      break;
    case SymbolType::EXTERNAL:
      os << "E";
      break;
    }
    os << "\n";
  };

  os << "CSect   Symbol  Value   LENGTH  Flags:\n";
  os << "--------------------------------------\n";

  print_csect(os, m_name, m_start_address, length_in_bytes());
  for (const auto &label : m_symbols) {
    if (label.first == m_name) {
      continue;
    }

    print_symbol(os, label.first, label.second);
  }

  const auto print_literal = [](std::ostream &os, const std::string &name,
                                const std::shared_ptr<LiteralInfo> &info) {
    os << std::setw(7) << std::setfill(' ') << std::left << name;
    os << " ";

    for (const auto &byte : info->bytes()) {
      os << std::setw(2) << std::setfill('0') << std::right << std::hex
         << (int)byte;
    }

    os << "\t";

    os << std::setw(6) << std::setfill('0') << std::right << std::hex
       << info->address();
    os << " ";
    os << std::setw(0) << std::dec << info->length();
    os << "\n";
  };

  os << "\n";
  os << "Literal Table \n";
  os << "Name    Operand   Address  Length:\n";
  os << "--------------------------------\n";

  for (const auto &literal : m_literals) {
    print_literal(os, literal.first, literal.second);
  }
}

void ControlSection::assembler_error(const std::string &message) const {
  std::cerr << m_path;
  std::cerr << ": error: " << message << "\n";

  std::exit(1);
}

void ControlSection::assembler_error(const std::string &message,
                                     const Statement &stmt) const {
  std::cerr << m_path;

  if (stmt.line_number() != -1) {
    std::cerr << ":" << stmt.line_number();
  }
  std::cerr << ": error: " << message << "\n";

  std::cerr << "at statement:\n";
  std::cerr << stmt.raw() << "\n";

  std::exit(1);
}
