#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>

#include "ControlSection.hh"
#include "Directives.hh"
#include "Statement.hh"

ControlSection::ControlSection(std::string path)
    : m_path(path), m_location_counter(-1), m_start_address(-1),
      m_end_address(-1), m_current_pass(Pass::NONE) {
  std::cerr << "ControlSection::ControlSection(" << path << ")" << std::endl;

  // Read the file at path into m_raw
  std::ifstream i_file(path);

  std::basic_ostringstream<char> os;
  os << i_file.rdbuf();
  m_raw = os.str();

  std::cerr << "Read " << m_raw.size() << " bytes from " << path << std::endl;
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
    } else if (stmt.type() == StatementType::LITERAL) {
      // Handle literals
      // std::cerr << "Literal: " << statement << std::endl;
    } else if (stmt.type() == StatementType::INVALID) {
      assert(false && "Invalid statement");
    } else {
      // Handle instructions
      // std::cerr << "Instruction: " << stmt.opcode() << std::endl;
    }

    // Make sure the location counter was set by START directive
    assert(m_location_counter != -1);

    if (stmt.has_label() && stmt.type() != StatementType::LITERAL) {
      m_labels[stmt.label()] = m_location_counter;
      std::cerr << "Label: " << stmt.label() << " @ " << m_location_counter
                << std::endl;
    }

    m_statements.push_back(stmt);

    m_location_counter += stmt.size();
  }

  std::cerr << "ControlSection::first_pass() done" << std::endl;
}

// Pass 2 (assemble instructions and generate object program):
// 1. Assemble instructions (translating operation codes and looking up
// addresses).
// 2. Generate data values defined by BYTE, WORD, etc.
// 3. Perform processing of assembler directives not done during Pass 1.
// 4. Write the object program and the assembly listing.

void ControlSection::print() const {
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
    stmt.print_assembled(std::cerr);
  }

  std::cerr << "ControlSection::print() done" << std::endl;
}

void ControlSection::print_symbol_table(std::ostream &os) const {}