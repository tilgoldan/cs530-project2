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

enum class SymbolType {
  RELATIVE,
  ABSOLUTE,
  EXTERNAL,
};

class SymbolInfo {
public:
  SymbolInfo(int32_t value, SymbolType type) : m_value(value), m_type(type) {}

  int32_t value() const { return m_value; }
  SymbolType type() const { return m_type; }

private:
  int32_t m_value;
  SymbolType m_type;
};

using SymbolTab = std::map<std::string, std::shared_ptr<SymbolInfo>>;