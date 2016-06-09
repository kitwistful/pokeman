/*
* type_analyzer.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/
#include "type_analyzer.hpp"

#include <cassert>

#include <iostream>

#include "pokeman.hpp"
#include "resources.hpp"

namespace pokeman {
void TypeAnalyzer::analyze_against_type(Type type) const {
  assert(chart_ != nullptr);
  assert(type != kNullType);
  std::cout << "Analysis of " << resources::getTypeName(type) << ":" << std::endl;

  // give weaknesses
  std::cout << "Weaknesses:" << std::endl;
  std::list<Type> weaknesses = chart_->getDefensiveWeaknesses(type);
  for(Type weakness : weaknesses) {
    std::cout << "  " << resources::getTypeName(weakness) << std::endl;
  }

  // pokemans
  std::cout << "Team:" << std::endl;
  int nweak, nstrong;
  nweak = nstrong = 0;
  if(type != kNullType) {
    for(Monster monster : monsters_) {
      // determine suitability
      int suitability = evaluateSuitability(monster.species_->type_, type);
      std::string description;
      if(suitability < 0) {
        nweak++;
        description = "Weak.";
      } else if (suitability > 1) {
        nstrong++;
        description = "Strong!!";
      } else if (suitability == 1) {
        description = "Decent?";
      } else if (suitability == 0) {
        description = "Alright...";
      } else {
        description = "ERROR";
      }

      // print result
      std::cout << "  " << monster.name_ << "  " << monster.species_->name_ << "  " << description << std::endl;
    }
    std::cout << "Recommended: " << nstrong << std::endl
      << "Nullified: " << nweak << std::endl;
  }
  std::cout << std::endl;
}

void TypeAnalyzer::analyze_weaknesses(const size_t max_count) const {
  // calculate
  std::map<Type, int> weakness_counts;
  for (int i = 0; i < kNullType; i++) {
    int weakness_count = 0;
    for (Monster monster : monsters_) {
      if (evaluateSuitability(monster.species_->type_, (Type)i) < 0) {
        weakness_count++;
      }
    }
    weakness_counts[(Type)i] = weakness_count;
  }

  // reveal
  std::cout << "-- Weaknesses:" << std::endl;
  printRanking(weakness_counts, max_count);
  std::cout << std::endl;
}

void TypeAnalyzer::analyze_strengths(const size_t max_count) const {
  // calculate
  std::map<Type, int> strength_counts;
  for(int i = 0; i < kNullType; i++) {
    int strength_count = 0;
    for(Monster monster : monsters_) {
      if(evaluateSuitability(monster.species_->type_, (Type)i) > 0) {
        strength_count++;
      }
    }
    strength_counts[(Type)i] = strength_count;
  }

  // reveal
  std::cout << "-- Strengths:" << std::endl;
  printRanking(strength_counts, max_count);
  std::cout << std::endl;
}

bool TypeAnalyzer::addMonsterToTeam(const Monster & monster) {
  if(monster.species_ == nullptr) {
    std::cerr << "Could not add " << monster.name_ << " to team; invalid species" << std::endl;
    return false;
  } else {
    monsters_.push_back(monster);
    return true;
  }
}

int TypeAnalyzer::addMonstersToTeam(const std::vector<Monster>& monsters) {
  int nfails = 0;
  for(Monster monster : monsters) {
    if(!addMonsterToTeam(monster))
      nfails++;
  }
  return nfails;
}

int TypeAnalyzer::evaluateSuitability(const TypesHad & types, const Type type) const {
  TypeEffectiveness offense = chart_->getTypeEffectivenessXonY(types, type);
  TypeEffectiveness defense = chart_->getTypeEffectivenessXonY(type, types);
  if(typeEffectivenessIsStrong(defense)) {
    if(typeEffectivenessIsWeak(offense)) {
      return 2; // stronk
    } else {
      return 1; // no STAB
    }
  } else if (offense == kZeroTimes) {
    return -1; // not recommended
  } else if (typeEffectivenessIsWeak(defense)) {
    return -2; // severely not recommended
  } else {
    return 0; // it'll work OK
  }

}

std::vector<Type> TypeAnalyzer::rank(const std::map<Type, int>& values) {
  // rank
  assert(!values.empty());
  std::vector<Type> types_ranked;
  for(int i = 0; i < kNullType; i++) {
    const Type type = (Type)i;
    const int size = (int)types_ranked.size();
    for(int j = 0; j <= size; j++) {
      if(j == size) {
        types_ranked.push_back(type);
      } else {
        Type type_in_slot = types_ranked[j];
        assert(values.find(type_in_slot) != values.end());
        assert(values.find(type) != values.end());
        bool greater = values.at(type) > values.at(type_in_slot);
        if(greater) {
          types_ranked.insert(types_ranked.begin() + j, type);
          break;
        }
      }
    }
  }

  return types_ranked;
}

void TypeAnalyzer::printRanking(const std::map<Type, int>& values, const size_t max_count) {
  assert(!values.empty());
  const std::vector<Type> ranks = rank(values);
  unsigned int count = 0;
  for(Type type : ranks) {
    int value = values.at(type);
    if(value <= 0 || count + 1 == max_count) {
      break;
    } else {
      count++;
      std::cout << resources::getTypeName(type) << " " << value << std::endl;
    }
  }
}

} //namespace pokeman