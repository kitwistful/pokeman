/*
* moveset_analysis.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*
* Driver that, given a team of pokemon, identifies their weaknesses.
*/
#include "moveset_analysis.hpp"

#include <cassert>

#include <iostream>

#include "resources.hpp"
#include "pokeman_loader.hpp"

#define MOVESET_ANALYSIS_FPATH_TEAM "Data/team.yml"
#define MOVESET_ANALYSIS_FPATH_SPECIES "Data/species.yml"
#define MOVESET_ANALYSIS_FPATH_MOVES "Data/moves.yml"

#define MOVESET_FIRST_QUADRANT 0
#define MOVESET_SECOND_QUADRANT 1
#define MOVESET_THIRD_QUADRANT 2
#define MOVESET_FOURTH_QUADRANT 3
#define MOVESET_QUADRANT_MOVES_TO_LIST 10

using namespace std::literals::string_literals;

namespace pokeman {
namespace driver {
/// Finds moves that are strong against the types that the monster resists well.
/// This includes status-type moves. Moves that are used on self are
/// always included.
std::vector<std::string> MovesetAnalyzer::getStrongMoves(const TypeChart& chart, const Monster& monster) const {
  assert(monster.species_ != nullptr);
  std::list<Type> types_of_interest = chart.getDefensiveStrengths(monster.species_->type_, true);
  std::vector<std::string> move_pool = monster.species_->getMovepool();
  std::vector<std::string> strong_moves;
  for(std::string move_name : move_pool) {
    Move move;
    if(!getMoveIfExists(move_name, &move)) {
      std::cerr << "could not get move '" << move_name << "'" << std::endl;
      return std::vector<std::string>();
    } else {
      for(Type type : types_of_interest) {
        if(chart.isWeakTo(type, move.elemental_type_) || move.move_type_ == kStatus) {
          strong_moves.push_back(move_name);
          break;
        }
      }
    }
  }
  return strong_moves;
}

/// Filters out moves based on type.
std::vector<std::string> MovesetAnalyzer::filterMovesByType(const std::vector<std::string>& moves, const MoveType type) const {
  assert(type != kNullMoveType);
  if(type == kNullMoveType) {
    std::cerr << "Could not filter moves by null type" << std::endl;
    return std::vector<std::string>();
  } else {
    std::vector<std::string> filtered;
    for(std::string move_name : moves) {
      Move move;
      if(!getMoveIfExists(move_name, &move)) {
        std::cerr << "Error retrieving move '" << move_name << "'" << std::endl;
        return std::vector<std::string>();
      } else {
        if(move.move_type_ == type) {
          filtered.push_back(move_name);
        }
      }
    }
    return filtered;
  }
}

std::vector<std::string> MovesetAnalyzer::filterMovesByStab(const std::vector<std::string>& moves, const TypesHad & typing) const {
  std::vector<std::string> stab;
  for(std::string move_name : moves) {
    Move move;
    if(!getMoveIfExists(move_name, &move)) {
      std::cerr << "Error: Not able to find move '" << move_name << std::endl;
      return std::vector<std::string>();
    } else {
      if(isStab(move, typing)) {
        stab.push_back(move_name);
      }
    }
  }
  return stab;
}

std::vector<std::string> MovesetAnalyzer::filterMovesByNonDamaging(const std::vector<std::string>& moves) const {
  std::vector<std::string> nondamaging;
  for(std::string move_name : moves) {
    Move move;
    if(!getMoveIfExists(move_name, &move)) {
      std::cerr << "Error: Not able to find move '" << move_name << std::endl;
      return std::vector<std::string>();
    } else {
      if(isNonDamaging(move)) {
        nondamaging.push_back(move_name);
      }
    }
  }
  return nondamaging;
}

std::vector<std::string> MovesetAnalyzer::filterStatChangeMovesByMonster(const std::vector<std::string>& moves, const Monster & monster) const {
  std::vector<std::string> moves_filtered;
  for(std::string move_name : moves) {
    Move move;
    if(!getMoveIfExists(move_name, &move)) {
      std::cerr << "Error: Not able to find move '" << move_name << std::endl;
      return std::vector<std::string>();
    } else {
      if (move.stat_modifiers_self_.attack_ > 0 && suitedToPhysical(monster)) {
        moves_filtered.push_back(move_name);
      } else if (move.stat_modifiers_self_.special_attack_ > 0 && suitedToSpecial(monster)) {
        moves_filtered.push_back(move_name);
      }
    }
  }
  return moves_filtered;
}

std::vector<std::string> MovesetAnalyzer::filterMovesByUtility(const std::vector<std::string>& moves, const Monster & monster) const {
  std::vector<std::string> utility;
  for(std::string move_name : moves) {
    Move move;
    if(!getMoveIfExists(move_name, &move)) {
      std::cerr << "Error: Not able to find move '" << move_name << std::endl;
      return std::vector<std::string>();
    } else {
      if(isUtility(move)) {
        utility.push_back(move_name);
      }
    }
  }
  return utility;
}

std::list<Type> MovesetAnalyzer::getMoveSupereffectiveTypes(const TypeChart & chart, const std::string & move_name) const {
  Move move;
  if(!getMoveIfExists(move_name, &move)) {
    std::cerr << "Error: could not find move '" << move_name << "'" << std::endl;
    return std::list<Type>();
  } else if (!MoveTarget(move.target_).target_foe) {
    return std::list<Type>(); // cannot be practically supereffective  
  } else {
    return chart.getOffensiveStrengths(move.elemental_type_, false);
  }
}

std::string MovesetAnalyzer::moveToString(std::string move_name) const {
  Move move;
  if(!getMoveIfExists(move_name, &move)) {
    return move_name + " <NOT FOUND>"s;
  } else {
    return "'"s + move.name_ + "' "s + resources::getTypeName(move.elemental_type_) + " "s + resources::getMoveTypeName(move.move_type_) + " "s
      + std::to_string(move.pp_) + "mp " +  (move.power_ > 0 ? std::to_string(move.power_) : "--")
      + " "s + (move.accuracy_ > 0 ? std::to_string(move.accuracy_) : "--") + "%"s;
  }
}

void MovesetAnalyzer::printMovesetAnalysis(const TypeChart & chart, const Monster & monster) const {
  assert(monster.species_ != nullptr);

  // get moveset
  std::vector<std::string> moves = getStrongMoves(chart, monster);

  // print name
  std::cout << monster.toString() << ": " << std::endl;

  // print best moves
  if(monster.species_->type_.b_dual_type_) {
    printFourMovePicksForDualtyped(moves, monster);
  } else {
    printFourMovePicksForMonotyped(moves, monster);
  }
}

bool MovesetAnalyzer::getMoveIfExists(const std::string & string, Move * move_dest) const {
  assert(move_dest != nullptr);
  if(move_data_.find(string) == move_data_.end()) {
    return false;
  } else {
    *move_dest = move_data_.at(string);
    return true;
  }
}

void MovesetAnalyzer::printFourMovePicksForMonotyped(const std::vector<std::string>& moves, const Monster & monster) const {
  assert(monster.species_ != nullptr);
  assert(!monster.species_->type_.b_dual_type_);

  // check how many HMS are taken
  std::bitset<4> quadrant_taken = analyzeHMniches(monster);

  // These are the ranked moves decided on.

  std::map<std::string, double> ranking_map = getMoveRatings(moves, monster);
  MoveRankings rankings(&ranking_map, this);
  Type type = monster.species_->type_.first_type_;
  std::vector<std::vector<std::string>> slots;
  slots.push_back(getPicks(rankings.pickStabWithHighPP(type), monster, quadrant_taken[MOVESET_FIRST_QUADRANT], MOVESET_FIRST_QUADRANT));
  slots.push_back(getPicks(rankings.pickStabWithHighPower(type), monster, quadrant_taken[MOVESET_SECOND_QUADRANT], MOVESET_SECOND_QUADRANT));
  slots.push_back(getPicks(rankings.pickNonStabButStrong(monster.species_->type_), monster, quadrant_taken[MOVESET_THIRD_QUADRANT], MOVESET_THIRD_QUADRANT));
  slots.push_back(getPicks(rankings.pickNonDamaging(monster), monster, quadrant_taken[MOVESET_FOURTH_QUADRANT], MOVESET_FOURTH_QUADRANT));

  // Print results
  std::vector<MovesetPicksNode> meta;
  meta.push_back({"Bread n Butter STAB", quadrant_taken[meta.size()]});
  meta.push_back({"High Powered STAB", quadrant_taken[meta.size()]});
  meta.push_back({"Utility", quadrant_taken[meta.size()]});
  meta.push_back({"Non damaging", quadrant_taken[meta.size()]});
  printPicks(slots, meta);

}

void MovesetAnalyzer::printFourMovePicksForDualtyped(const std::vector<std::string>& moves, const Monster & monster) const {
  assert(monster.species_ != nullptr);
  assert(monster.species_->type_.b_dual_type_);

  // check how many HMS are taken
  std::bitset<4> quadrant_taken = analyzeHMniches(monster);

  // These are the ranked moves decided on.
  std::map<std::string, double> ranking_map = getMoveRatings(moves, monster);
  MoveRankings rankings(&ranking_map, this);
  Type first_type = monster.species_->type_.first_type_;
  Type second_type = monster.species_->type_.second_type_;
  std::vector<std::vector<std::string>> slots;
  slots.push_back(getPicks(rankings.pickStabWithHighPP(first_type), monster, quadrant_taken[MOVESET_FIRST_QUADRANT], MOVESET_FIRST_QUADRANT));
  slots.push_back(getPicks(rankings.pickStabWithHighPP(second_type), monster, quadrant_taken[MOVESET_SECOND_QUADRANT], MOVESET_SECOND_QUADRANT));
  slots.push_back(getPicks(rankings.pickNonStabButStrong(monster.species_->type_), monster, quadrant_taken[MOVESET_THIRD_QUADRANT], MOVESET_THIRD_QUADRANT));
  slots.push_back(getPicks(rankings.pickNonDamaging(monster), monster, quadrant_taken[MOVESET_FOURTH_QUADRANT], MOVESET_FOURTH_QUADRANT));

  // Print results
  std::vector<MovesetPicksNode> meta;
  meta.push_back({"First type STAB", quadrant_taken[meta.size()]});
  meta.push_back({"Second type STAB", quadrant_taken[meta.size()]});
  meta.push_back({"Utility", quadrant_taken[meta.size()]});
  meta.push_back({"Non damaging", quadrant_taken[meta.size()]});
  printPicks(slots, meta);
}

std::vector<std::string> MovesetAnalyzer::getPicks(const std::vector<std::string>& picks, const Monster & monster, const bool isHM, const unsigned int slot) const {
  std::vector<std::string> moves;
  if(!isHM) {
    if(picks.size() <= MOVESET_QUADRANT_MOVES_TO_LIST) {
      return picks;
    } else {
      moves.assign(picks.cbegin(), picks.cbegin() + (MOVESET_QUADRANT_MOVES_TO_LIST));
    }
  } else {
    assert(monster.hm_moves_.size() >= slot + 1);
    moves.push_back(monster.hm_moves_[slot]);
  }
  return moves;
}

void MovesetAnalyzer::printPicks(const std::vector<std::vector<std::string>>& picks, const std::vector<MovesetPicksNode>& meta) const {
  assert(picks.size() == meta.size());
  assert(picks.size() == 4);

  // print descriptions or note lock for each pick.
  for(int i = 0;  i < 4; i++) {
    MovesetPicksNode info = meta[i];
    std::cout << "  (" << (i + 1) << ") " << (info.isHM ? "LOCKED"s : info.description) << ":" << std::endl;
    for(std::string pick : picks[i]) {
      std::cout << "      " << moveToString(pick) << std::endl;
    }
  }
  std::cout << std::endl;
}

std::map<std::string, double> MovesetAnalyzer::getMoveRatings(const std::vector<std::string>& moves, const Monster & monster) const {
  std::map<std::string, double> ratings;
  for(std::string move_name : moves) {
    Move move;
    if(!getMoveIfExists(move_name, &move)) {
      std::cerr << "Error compiling move priorities on '" << move_name << "'" << std::endl;
      return std::map<std::string, double>();
    } else {
      ratings[move_name] = moveRating(monster, move);
    }
  } 
  return ratings;
}

std::bitset<4> MovesetAnalyzer::analyzeHMniches(const Monster & monster) {
  assert(monster.hm_moves_.size() < 5);
  std::bitset<4> quadrants;
  for(unsigned int i = 0; i < monster.hm_moves_.size(); i++) {
    quadrants[i] = true; //todo: actually check niches, after tests pass and the demo otherwise works
  }
  return quadrants;
}

bool MovesetAnalyzer::isStab(const Move move, const TypesHad & typing) {
  if(!MoveTarget(move.target_).target_foe || isNonDamaging(move)) {
    return false;
  } else {
    bool is_stab_first = move.elemental_type_ == typing.first_type_;
    bool is_stab_second = typing.b_dual_type_ && move.elemental_type_ == typing.second_type_;
    return is_stab_first || is_stab_second;
  }
}

bool MovesetAnalyzer::isNonDamaging(const Move move) {
  return move.power_ < 0 || move.move_type_ == kStatus;
}

bool MovesetAnalyzer::isUtility(const Move move) {
  return isNonDamaging(move) && !hasStatModifier(move);
}

bool MovesetAnalyzer::hasStatModifier(const Move move) {
  return !move.stat_modifiers_self_.isBlank() || !move.stat_modifiers_target_.isBlank();
}

double MovesetAnalyzer::accuracyRating(const Move move) {
  if(move.accuracy_ < 0) {
    return 1.0; // doesn't miss
  } else {
    return (double)(move.accuracy_ - 100)/10;
  }
}

double MovesetAnalyzer::moveRating(const Monster & monster, const Move move) {
  if(!suitedToMoveTypeRating(monster, move)) {
    return 0.0;
  } else if (move.move_type_ == kStatus) {
    return 0.8;
  } else {
    double moveAccuracyPP = accuracyRating(move) * ((double)move.pp_ / 20);
    double movePower = (double)move.power_ / 90;
    return std::fmin(moveAccuracyPP * movePower, 1.0);
  }
}

bool MovesetAnalyzer::suitedToPhysical(const Monster & monster) {
  return monster.species_->base_stats_.attack_ >= monster.species_->base_stats_.special_attack_;
}

bool MovesetAnalyzer::suitedToSpecial(const Monster & monster) {
  return monster.species_->base_stats_.special_attack_ >= monster.species_->base_stats_.attack_;
}

bool MovesetAnalyzer::suitedToMoveTypeRating(const Monster & monster, const Move move) {
  bool isPhysical = move.move_type_ == kPhysical && suitedToPhysical(monster);
  bool isSpecial = move.move_type_ == kSpecial && suitedToSpecial(monster);
  bool isStatus = move.move_type_ == kStatus;
  return isPhysical || isSpecial || isStatus;
}


int moveset_analysis(int argc, const char * argv[]) {
  // initialize config
  resources::Loader config_loader(FILEPATH_CONFIG);
  if(config_loader.errorOccured()) {
    std::cerr << "Failed to load config, exiting now" << std::endl;
    return 1;
  }

  // load stuff
  bool error_occured;
  resources::PokemanDatabase database = resources::initialize(&error_occured);
  if(error_occured) {
    std::cerr << "Couldn't load database; Exiting now" << std::endl;
    return 2;
  }

  // develop moveset data
  MovesetAnalyzer analyzer;
  analyzer.move_data_ = database.getMoves();

  // get moves for each pokemon
  TypeChart chart = resources::generateTypeChartGen5();
  std::cout << "--[Find moves for each dude:] --" << std::endl;
  for(Monster monster : database.getTeam()) {
    analyzer.printMovesetAnalysis(chart, monster);
  }

  // done
  return 0;
}

MoveRankings::MoveRankings(const std::map<std::string, double>* rankings, const MovesetAnalyzer * analyzer) :
  rankings_(rankings), analyzer_(analyzer) {
  assert(rankings_ != nullptr);
  assert(analyzer_ != nullptr);
}

std::vector<std::string> MoveRankings::pickStabWithHighPP(Type type) {
  // get moves list
  std::vector<std::string> moves = getMovesRatedAtLeast();
  moves = analyzer_->filterMovesByStab(moves, type);
  if(moves.empty()) {
    return std::vector<std::string>();
  } else {
    return moves; // todo: actually filter by high PP somehow, however that would work 8/
  }
}

std::vector<std::string> MoveRankings::pickStabWithHighPower(Type type) {
  return pickStabWithHighPP(type); //todo: fix, after I figure out how to prioritize this 8/
}

std::vector<std::string> MoveRankings::pickNonStabButStrong(TypesHad typing) {
  // get moves list
  std::vector<std::string> moves = getMovesRatedAtLeast();
  std::vector<std::string> stab_moves = analyzer_->filterMovesByStab(moves, typing);
  std::vector<std::string> nondamaging_moves = analyzer_->filterMovesByNonDamaging(moves);
  if(moves.empty()) {
    return std::vector<std::string>();
  } else if (stab_moves.empty()) {
    return moves;
  } else {
    std::vector<std::string> nonstab_moves;
    for(int i = 0; i < (int)moves.size(); i++) {
      if(!moveExistsInVector(stab_moves, moves[i]) && !moveExistsInVector(nondamaging_moves, moves[i])) {
        nonstab_moves.push_back(moves[i]);
      }
    }
    return nonstab_moves;
  }
}

std::vector<std::string> MoveRankings::pickNonDamaging(const Monster& monster) {
  // get moves list
  std::vector<std::string> moves = getMovesRatedAtLeast();
  moves = analyzer_->filterMovesByNonDamaging(moves);
  
  // filter down two kinds of moves
  std::vector<std::string> stat_changes = analyzer_->filterStatChangeMovesByMonster(moves, monster);
  std::vector<std::string> unique = analyzer_->filterMovesByUtility(moves, monster);
  moves.clear();
  moves.insert(moves.begin(), stat_changes.begin(), stat_changes.end());
  moves.insert(moves.begin(), unique.begin(), unique.end());

  if(moves.empty()) {
    return std::vector<std::string>();
  } else {
    return moves;
  }
}

std::vector<std::string> MoveRankings::getMovesRatedAtLeast(double min_rating) {
  std::vector<std::string> moves;
  for(std::pair<std::string, double> pair : *rankings_) {
    if(pair.second >= min_rating)
      moves.push_back(pair.first);
  }
  return moves;
}

std::vector<std::string> MoveRankings::getMovesSorted(double min_rating) {
  return std::vector<std::string>(); // todo: later
}

bool MoveRankings::moveExistsInVector(const std::vector<std::string>& vector, const std::string & value) {
  for(std::string string : vector) {
    if(value.compare(string) == 0)
      return true;
  }
  return false;
}
}
}
