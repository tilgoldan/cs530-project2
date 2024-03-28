#include <cassert>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>

#include "ControlSection.hh"
#include "Directives.hh"
#include "Statement.hh"

ControlSection::ControlSection(std::string path)
    : m_path(path), m_location_counter(-1), m_current_pass(Pass::NONE),
      m_start_address(-1), m_end_address(-1) {
  std::cerr << "ControlSection::ControlSection(" << path << ")" << std::endl;

  // Read the file at path into m_raw
  std::ifstream i_file = std::ifstream(path);

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

  std::istringstream i_stream = std::istringstream(m_raw);
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

    Statement statement = Statement(line, m_location_counter);

    statement.print();

    if (statement.type() == StatementType::DIRECTIVE) {
      // Handle directives
      std::cerr << "executing directive: " << statement.opcode() << std::endl;
      auto handler = directives[statement.opcode()];
      // This might modify the statement and the control section
      handler(statement, *this);
    } else if (statement.type() == StatementType::INVALID) {
      assert(false && "Invalid statement");
    } else {
      // Handle instructions
      std::cerr << "Instruction: " << statement.opcode() << std::endl;
    }

    // Make sure the location counter was set by START directive
    assert(m_location_counter != -1);

    if (statement.has_label()) {
      m_labels[statement.label()] = m_location_counter;
      std::cerr << "Label: " << statement.label() << " @ "
                << m_location_counter << std::endl;
    }

    m_location_counter += statement.size();
  }

  std::cerr << "ControlSection::first_pass() done" << std::endl;
}

// Pass 2 (assemble instructions and generate object program):
// 1. Assemble instructions (translating operation codes and looking up
// addresses).
// 2. Generate data values defined by BYTE, WORD, etc.
// 3. Perform processing of assembler directives not done during Pass 1.
// 4. Write the object program and the assembly listing.