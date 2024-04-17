/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Symbol.hh
*/
#pragma once

#include <map>
#include <string>

enum class Symbol {
  RELATIVE,
  ABSOLUTE,
};

class SymbolInfo {
public:
  SymbolInfo(int32_t address, Symbol type)
      : m_address(address), m_type(type) {}

  int32_t address() const { return m_address; }
  Symbol type() const { return m_type; }

  bool external() const { return m_external; }
  void is_external(bool external) { m_external = external; }

private:
  int32_t m_address;
  Symbol m_type;
  bool m_external;
};

using SymbolTab = std::map<std::string, std::shared_ptr<SymbolInfo>>;