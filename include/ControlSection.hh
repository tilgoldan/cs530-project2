#pragma once

#include <map>
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

  int32_t location_counter() const { return m_location_counter; }
  void set_location_counter(int32_t location) { m_location_counter = location; }

  Pass current_pass() const { return m_current_pass; }


private:
  const std::string m_path;
  std::string m_raw;
  std::string m_name;

  std::map<std::string, int32_t> m_labels;
  int32_t m_location_counter;

  Pass m_current_pass;
};