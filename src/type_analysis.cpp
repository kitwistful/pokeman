/*
* type_analysis.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/
#include "type_analysis.hpp"

#include <iostream>

#include "pokeman_loader.hpp"
#include "type_analyzer.hpp"
#include "resources.hpp"

#define NUMBER_TO_RANK 5
#define FPATH_TEAM "Data/team.yml"

namespace pokeman {
namespace driver {
int type_analysis(int argc, const char* argv[]) {
  // get database
  bool error_occured;
  resources::PokemanDatabase data = resources::initialize(&error_occured);
  if(error_occured) {
    std::cout << "couldn't load database, closing now" << std::endl;
    return 1;
  }

  // pick a team style
  const std::vector<Monster>& team = data.getTeam();

  // print team
  std::cout << "--[ Team ]--" << std::endl;
  for(Monster monster : team) {
    std::cout << monster.toString() << std::endl;
  }
  std::cout << std::endl;

  // put into TypeAnalyzer
  TypeAnalyzer analyzer;
  analyzer.chart_ = &data.getChart();
  analyzer.addMonstersToTeam(team);

  // print general analysis
  analyzer.analyze_weaknesses(NUMBER_TO_RANK);
  analyzer.analyze_strengths(NUMBER_TO_RANK);

  // print analysis for each element among argc
  if(argc > 2) {
    for(int i = 2; i < argc; i++) {
      Type type = resources::getTypeCode(argv[i]);
      if(type == kNullType) {
        std::cerr << "[Error] '" << argv[i] << "' was not recognized as a type" << std::endl;
        break;
      } else {
        analyzer.analyze_against_type(resources::getTypeCode(argv[i]));
      }
    }
  }

  // done
  return 0;
}

} // namespace driver
} // namespace pokeman  