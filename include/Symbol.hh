/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Label.hh
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

private:
  int32_t m_address;
  Symbol m_type;
};

using SymbolTab = std::map<std::string, std::shared_ptr<SymbolInfo>>;