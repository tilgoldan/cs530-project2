/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Literal.hh
*/
#pragma once

#include <string>
#include <memory>
#include <vector>

class Literal {
public:
  Literal(std::string name, std::shared_ptr<std::vector<uint32_t>> values,
          int32_t address)
      : m_name(name), m_values(values), m_address(address) {
    m_values = std::make_shared<std::vector<uint32_t>>();
  }

  const std::string &name() const { return m_name; }
  const std::vector<uint32_t> &values() const { return *m_values; }
  int32_t address() const { return m_address; }
  int32_t length() const { return m_values->size(); }

private:
  std::string m_name;
  std::shared_ptr<std::vector<uint32_t>> m_values;
  int32_t m_address;
};