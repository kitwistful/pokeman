/*
* moveset_analysis.hpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*
* Driver that, given a team of pokemon, identifies their weaknesses.
*/
#ifndef POKEMAN_MOVESET_ANALYSIS_HPP_
#define POKEMAN_MOVESET_ANALYSIS_HPP_

#include <map>
#include <string>

#include "pokeman.hpp"

namespace pokeman {
namespace driver {
struct MovesetPicksNode {
  std::string description;
  bool isHM;
};

class MovesetAnalyzer {
public:
  std::map<std::string, Move> move_data_;

public:
  
  std::vector<std::string> getStrongMoves(const TypeChart& chart, const Monster& monster) const;

  std::vector<std::string> filterMovesByType(const std::vector<std::string>& moves, const MoveType type) const;

  std::vector<std::string> filterMovesByStab(const std::vector<std::string>& moves, const TypesHad& typing) const;

  std::vector<std::string> filterMovesByNonDamaging(const std::vector<std::string>& moves) const;

  /// Seeks out moves that have a buff towards the monster.
  std::vector<std::string> filterStatChangeMovesByMonster(const std::vector<std::string>& moves, const Monster& monster) const;
  
  /// Seeks out moves that have some unquantifiable utility
  std::vector<std::string> filterMovesByUtility(const std::vector<std::string>& moves, const Monster& monster) const;

  std::list<Type> getMoveSupereffectiveTypes(const TypeChart& chart, const std::string& move_name) const;

  std::string moveToString(std::string move_name) const;

  void printMovesetAnalysis(const TypeChart& chart, const Monster& monster) const;

private:
  bool getMoveIfExists(const std::string& string, Move* move_dest) const;

  void printFourMovePicksForMonotyped(const std::vector<std::string>& moves, const Monster& monster) const;

  void printFourMovePicksForDualtyped(const std::vector<std::string>& moves, const Monster& monster) const;

  /// picks must be sorted.
  std::vector<std::string> getPicks(const std::vector<std::string>& picks, const Monster& monster, const bool isHM, const unsigned int slot) const;

  void printPicks(const std::vector<std::vector<std::string>>& picks, const std::vector<MovesetPicksNode>& meta) const;

  std::map<std::string, double> getMoveRatings(const std::vector<std::string>& moves, const Monster& monster) const;

  static std::bitset<4> analyzeHMniches(const Monster& monster);

  static bool isStab(const Move move, const TypesHad& typing);

  static bool isNonDamaging(const Move move);

  static bool isUtility(const Move move);

  static bool hasStatModifier(const Move move);

  static double accuracyRating(const Move move);

  static double moveRating(const Monster& monster, const Move move);
  
  static bool suitedToPhysical(const Monster& monster);

  static bool suitedToSpecial(const Monster& monster);

  static bool suitedToMoveTypeRating(const Monster& monster, const Move move);

};

class MoveRankings {
private:
  const std::map<std::string, double>* rankings_;
  const MovesetAnalyzer* analyzer_;

public:
  MoveRankings(const std::map<std::string, double>* rankings, const MovesetAnalyzer* analyzer);

  std::vector<std::string> pickStabWithHighPP(Type type);

  std::vector<std::string> pickStabWithHighPower(Type type);

  std::vector<std::string> pickNonStabButStrong(TypesHad typing);

  std::vector<std::string> pickNonDamaging(const Monster& monster);

private:
  std::vector<std::string> getMovesRatedAtLeast(double min_rating = 0.0);

  std::vector<std::string> getMovesSorted(double min_rating = 0.0);

  static bool moveExistsInVector(const std::vector<std::string>& vector, const std::string& value);

};

int moveset_analysis(int argc, const char* argv[]);
} // namespace driver
} // namespace pokeman

#endif //POKEMAN_MOVESET_ANALYSIS_HPP_