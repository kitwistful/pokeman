/*
* test_core.hpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/

#include "test_core.hpp"

#include <iostream>

namespace pokeman {
namespace test {
int runTests(const TestNode* suite) {
  const TestNode* test = suite;
  int failures = 0;
  while(test->function != nullptr) {
    int result = test->function();
    if(result != 0) {
      std::cerr << "!!Failure on test '" << test->name << "'" << std::endl
                << "  Purpose of test: " << test->description << std::endl
                << "  Test Status: " << result << std::endl << std::endl;
      failures++;
    }
    test++;
  }
  return failures;
}
const char* getName(const NameEnumHarness * harness, const int k, const int null_value) {
  const NameEnumHarness* name = &harness[0];
  while (name->number != null_value) {
    if (name->number == k) {
      return name->name;
    }
    else {
      name++;
    }
  }
  return name->name;
}
} // namespace test
} // namespace pokeman