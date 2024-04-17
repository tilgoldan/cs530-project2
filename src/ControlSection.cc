/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: ControlSection.cc
*/

#include <cassert>
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

ControlSection::ControlSection(std::string path)
    : m_path(path), m_location_counter(-1), m_start_address(-1),
      m_end_address(-1), m_current_pass(Pass::NONE) {
  std::cerr << "ControlSection::ControlSection(" << path << ")" << std::endl;

  // Check path ends with '.sic'
  if (path.size() < 4 || path.substr(path.size() - 4) != ".sic") {
    std::cerr << "Invalid path: " << path << std::endl;
    assert(false && "Invalid file extension");
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

    std::cerr << "Defining pending literal: " << it.first << std::endl;

    std::stringstream ss;

    ss << "*        =";
    ss << it.first;

    std::string line = ss.str();

    Statement stmt = Statement(line, m_location_counter);
    m_location_counter += stmt.size();
    it.second->set_address(stmt.location());
    it.second->set_bytes(stmt.bytes());
    m_statements.push_back(stmt);
  }
}

std::shared_ptr<SymbolInfo>
ControlSection::define_symbol(const std::string name, int32_t address,
                              SymbolType type) {
  return m_labels[name] = std::make_shared<SymbolInfo>(address, type);
}

std::shared_ptr<SymbolInfo>
ControlSection::get_symbol(const std::string name) const {
  if (m_labels.find(name) != m_labels.end()) {
    return m_labels.at(name);
  }

  return nullptr;
}

// Pass 1 (define symbols) :
// 1. Assign addresses to all statements in the program.
// 2. Save the values(addresses) assigned to all labels for use in Pass .2
// 3. Perform some processing of assembler directives. (This includes processing
// that affects address assignment, such as determining the length of data areas
// defined by BYTE, RESW, etc.)

// Symbol length = 6 Byte

void ControlSection::first_pass() {
  std::cerr << "ControlSection::first_pass()" << std::endl;

  m_current_pass = Pass::FIRST;

  std::istringstream i_stream(m_raw);
  std::string line;

  auto &directives = Directives::the();

  while (std::getline(i_stream, line)) {

    // Skip empty lines
    if (line.empty()) {
      continue;
    }

    // Skip comments
    if (line[0] == '.') {
      continue;
    }

    std::cerr << line << "\n";

    Statement stmt = Statement(line, m_location_counter);

    // stmt.print_info();

    if (stmt.type() == StatementType::DIRECTIVE) {
      // Handle directives
      std::cerr << "executing directive: " << stmt.opcode() << std::endl;
      auto handler = directives[stmt.opcode()];
      // This might modify the statement and the control section
      handler(stmt, *this);
    } else if (stmt.type() == StatementType::INVALID) {
      assert(false && "Invalid statement");
    }

    // Make sure the location counter was set by START directive
    assert(m_location_counter != -1);

    if (stmt.has_label() && stmt.type() != StatementType::LITERAL &&
        stmt.opcode() != "EQU") {

      if (get_symbol(stmt.label())) {
        std::cerr << "Duplicate label: " << stmt.label() << std::endl;
        assert(false && "Duplicate label");
      }

      define_symbol(stmt.label(), m_location_counter, SymbolType::RELATIVE);
      std::cerr << "Label: " << stmt.label() << " @ " << m_location_counter
                << std::endl;

    } else if (stmt.type() == StatementType::LITERAL) {
      // Save the value of the literal
      std::cerr << "Literal: " << stmt.opcode() << " @ " << stmt.location()
                << std::endl;
      auto &literal = m_literals[stmt.opcode()];
      literal->set_address(stmt.location());
      literal->set_bytes(stmt.bytes());
    }

    // Handle literals in the operand field
    if (stmt.operand_prefix() == '=' &&
        m_literals.find(stmt.operand()) == m_literals.end()) {
      // Save the literal for later
      m_literals[stmt.operand()] = std::make_shared<LiteralInfo>();
    }

    m_statements.push_back(stmt);
    m_location_counter += stmt.size();

    if (m_process_pending_literals) {
      define_pending_literals();
      m_process_pending_literals = false;
    }
  }

  std::cerr << "ControlSection::first_pass() done" << std::endl;
}

// Pass 2 (assemble instructions and generate object program):
// 1. Assemble instructions (translating operation codes and looking up
// addresses).
// 2. Generate data values defined by BYTE, WORD, etc.
// 3. Perform processing of assembler directives not done during Pass 1.
// 4. Write the object program and the assembly listing.

void ControlSection::second_pass() {
  std::cerr << "ControlSection::second_pass()" << std::endl;

  m_current_pass = Pass::SECOND;

  for (auto &stmt : m_statements) {
    if (stmt.type() == StatementType::DIRECTIVE) {
      auto &directives = Directives::the();
      auto handler = directives[stmt.opcode()];
      handler(stmt, *this);
      continue;
    } else if (stmt.type() == StatementType::INVALID) {
      assert(false && "Invalid statement");
    }

    // Assemble instructions

    if (stmt.type() == StatementType::LITERAL) {
      // Done in first pass
    } else {
      // Normal instruction
      // Assemble the statement
      stmt.assemble(m_labels, m_literals, m_base_relative_label);
    }
  }
}

void ControlSection::print_listing(std::ostream &os) const {
  std::cerr << "ControlSection::print()" << std::endl;

  std::cerr << "Path: " << m_path << std::endl;
  std::cerr << "Name: " << m_name << std::endl;
  std::cerr << std::hex;
  std::cerr << "Start address: " << m_start_address << std::endl;
  std::cerr << "End address: " << m_end_address << std::endl;

  std::cerr << "LABELS: " << std::endl;
  for (const auto &label : m_labels) {
    std::cerr << label.first << ":\t" << label.second << std::endl;
  }

  for (const auto &stmt : m_statements) {
    stmt.print_assembled(os);
  }

  std::cerr << "ControlSection::print() done" << std::endl;
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
      os << std::setw(6) << std::setfill(' ') << std::left << std::hex
         << "";
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
  for (const auto &label : m_labels) {
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