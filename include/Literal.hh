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
#include <vector>
#include <map>

class LiteralInfo {
public:
  LiteralInfo()
      : m_address(-1) {}
  LiteralInfo(int32_t address, const std::vector<uint8_t> bytes)
      : m_address(address), m_bytes(bytes) {}

  int32_t address() const { return m_address; }
  void set_address(int32_t address) { m_address = address; }

  const std::vector<uint8_t> &bytes() const { return m_bytes; }
  void set_bytes(const std::vector<uint8_t> &bytes) { m_bytes = bytes; }

  int32_t length() const { return m_bytes.size(); }

private:
  int32_t m_address;
  std::vector<uint8_t> m_bytes;
};

using LiteralTab = std::map<std::string, std::shared_ptr<LiteralInfo>>;