/*
* main.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/

#include "moveset_analysis.hpp"
#include "test_pokeman.hpp"
#include "test_resources.hpp"
#include "type_analysis.hpp"

#include <cstring>

#include <iostream>
#include <vector>


struct ArgumentType {
  const char* name;
  const char* description;
  int(*driver)(int, const char*[]);
};

int unrecognized_argument(int argc, const char* argv[]);
int command_list(int argc, const char* argv[]);
int run_tests(int argc, const char* argv[]);

static const ArgumentType arguments[] = {
  { "moveset", "find suggestions about moveset.", pokeman::driver::moveset_analysis},
  { "types", "run type analysis on pokemon.", pokeman::driver::type_analysis},
  { "test", "run tests on source code.", run_tests},
  { "help", "print command list.", command_list},
  { nullptr, nullptr, unrecognized_argument }
};

int run_tests(int argc, const char* argv[]) {
  // tests
  std::vector<const pokeman::test::TestNode*> suites = {
    pokeman::test::pokeman_suite(),
    pokeman::test::resource_suite()
  };

  // run all suites
  int fails = 0;
  for(const pokeman::test::TestNode* suite : suites) {
    int result = pokeman::test::runTests(suite);
    if(result != 0) {
      std::cerr << "Failures occured: " << result << std::endl << std::endl;
      fails++;
    }
  }

  // we did it?
  if(fails == 0)
    std::cout << "All tests passed! c:" << std::endl;
  return fails;  
}

int unrecognized_argument(int argc, const char* argv[]) {
  std::cerr << "argument '" << argv[1] << "' was not recognized" << std::endl
      << "please type 'help' for command list" << std::endl;
  return 1;
}

int command_list (int argc, const char* argv[]) {
  std::cout << "--[ Poke Man Command List ]--" << std::endl;
  const ArgumentType* argument = &arguments[0];
  while (argument->name != nullptr) {
    std::cout << argument->name << ": " << argument->description << std::endl;
    argument++;
  }
  std::cout << "--[ END ]--" << std::endl;
  return 0;
}

int main(int argc, const char* argv[]) {
  if(argc < 2) { // run default driver
    pokeman::driver::type_analysis(argc, argv);
  } else {
    const ArgumentType* argument = &arguments[0];
    while(argument->name != nullptr && std::strcmp(argument->name, argv[1]) != 0) {
      argument++;
    }
    argument->driver(argc, argv);
  }
  return 0;
}
