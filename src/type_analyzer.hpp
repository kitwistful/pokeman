/*
* type_analyzer.hpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*
* This class provides utility to check the strength of types against each
* other.
*/
#ifndef POKEMAN_TYPE_ANALYZER_HPP_
#define POKEMAN_TYPE_ANALYZER_HPP_

#include <vector>
#include <map>

#include "pokeman.hpp"

namespace pokeman {

class TypeAnalyzer {
public:
  const TypeChart* chart_;
  
private:
  std::vector<Monster> monsters_;

public:

  void analyze_against_type(Type type) const;

  void analyze_weaknesses(const size_t max_count) const;

  void analyze_strengths(const size_t max_count) const;

  /// adds pokemon to team
  /// prints an error message and returns false if the pokemon cannot be
  /// analyzed.
  bool addMonsterToTeam(const Monster& monster);

  /// Adds all pokemon in vector to team
  /// prints an error message and returns number of pokemon that could not be
  /// added.
  int addMonstersToTeam(const std::vector<Monster>& monsters);

private:
  /// If a pokemon is suitable in a type matchup, return > 0.
  /// If a pokemon is unsuitable, return < 0.
  /// If the pokemon is merely decent, return 0.
  int evaluateSuitability(const TypesHad& types, const Type type) const;

  /// Creates a ranking based on the map.
  static std::vector<Type> rank(const std::map<Type, int>& values);

  /// Prints out ranking
  static void printRanking(const std::map<Type, int>& values, const size_t max_count);
};
} // namespace pokeman



#endif //POKEMAN_TYPE_ANALYZER_HPP_