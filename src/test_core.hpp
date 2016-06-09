/*
* test_core.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*
* Contains functions that would aid in testing this source code.
*/

#ifndef POKEMAN_TEST_CORE_HPP_
#define POKEMAN_TEST_CORE_HPP_

namespace pokeman {
namespace test {
struct TestNode {
  const char* name;
  const char* description;
  int(*function)();
};

struct NameEnumHarness {
  int number;
  const char* name;
};

int runTests(const TestNode* suite);

const char* getName(const NameEnumHarness* harness, const int k, const int null_value);
}
}

#endif //POKEMAN_TEST_CORE_HPP_