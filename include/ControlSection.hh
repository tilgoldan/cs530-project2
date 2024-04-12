/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: ControlSection.hh
*/

#pragma once

#include "Statement.hh"
#include <map>
#include <ostream>
#include <string>
#include <vector>

enum struct Pass {
  NONE,
  FIRST,
  SECOND,
};

class ControlSection {
public:
  ControlSection(std::string);
  ~ControlSection() = default;

  void first_pass();
  void second_pass();

  int32_t location_counter() const { return m_location_counter; }
  void set_location_counter(int32_t location) { m_location_counter = location; }

  int32_t start_address() const { return m_start_address; }
  void set_start_address(int32_t address) { m_start_address = address; }

  int32_t end_address() const { return m_end_address; }
  void set_end_address(int32_t address) { m_end_address = address; }

  int32_t length_in_bytes() const { return m_end_address - m_start_address; }

  const std::string &name() const { return m_name; }
  void set_name(const std::string &name) { m_name = name; }

  const std::string &base_relative_label() const {
    return m_base_relative_label;
  }
  void set_base_relative_label(const std::string &label) {
    m_base_relative_label = label;
  }

  void define_label(const std::string, int32_t);
  int32_t get_label(const std::string) const;
  
  void process_pending_literals_next() { m_process_pending_literals = true; };

  Pass current_pass() const { return m_current_pass; }

  void print() const;

  void print_symbol_table(std::ostream&) const;

private:
  const std::string m_path;
  std::string m_raw;
  std::string m_name;

  std::vector<Statement> m_statements;

  std::map<std::string, int32_t> m_labels;

  std::map<std::string, int32_t> m_literals;
  std::vector<std::string> m_pending_literals;
  bool m_process_pending_literals;

  int32_t m_location_counter;
  int32_t m_start_address;
  int32_t m_end_address;

  Pass m_current_pass;

  std::string m_base_relative_label;

  void define_pending_literals();
};