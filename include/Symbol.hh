/*
Written-By: Til Goldan (131473651)
Written-By: Jasmine McDuffie (825467728)
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Symbol.hh
*/
#pragma once

#include <map>
#include <string>

enum class SymbolType {
  RELATIVE,
  ABSOLUTE,
  EXTERNAL,
};

class SymbolInfo {
public:
  SymbolInfo(int32_t value, SymbolType type) : m_value(value), m_type(type) {}

  int32_t value() const { return m_value; }
  void set_value(int32_t value) { m_value = value; }

  SymbolType type() const { return m_type; }
  void set_type(SymbolType type) { m_type = type; }

private:
  int32_t m_value;
  SymbolType m_type;
};

using SymbolTab = std::map<std::string, std::shared_ptr<SymbolInfo>>;