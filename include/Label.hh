/*
Written-By: Til Goldan
Written-By: Jasmine McDuffie
Class: CS530, Spring 2024
Assignment: 2
User: cssc4033
File: Label.hh
*/
#pragma once

#include <string>

enum class LabelType {
  RELATIVE,
  ABSOLUTE,
};

class Label {
public:
  Label(std::string name, int32_t address, LabelType type)
      : m_name(name), m_address(address), m_type(type) {}

  const std::string &name() const { return m_name; }
  int32_t address() const { return m_address; }
  LabelType type() const { return m_type; }

private:
  std::string m_name;
  int32_t m_address;
  LabelType m_type;
};