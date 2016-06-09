/*
* pokeman.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016 
*/

#include "pokeman.hpp"

#include <cassert>

#include <bitset>
#include <iostream>
#include <string>
#include <vector>

#include "resources.hpp"

using namespace std::literals::string_literals;

namespace pokeman {

TypeInfo::TypeInfo() {
  // clear 
  for(int i = 0; i < kNullType; i++) {
    defending_[(Type)i] = kOneTimes;
  }
  number_of_types_ = 1;
}

bool TypeInfo::isWeakAgainst(Type type) const {
  assert(type != kNullType);
  return defending_[type] > kOneTimes;
}

bool TypeInfo::isStrongAgainst(Type type) const {
  return defending_[type] < kOneTimes;
}

TypeInfo TypeInfo::dualType(TypeInfo firstType, TypeInfo secondType) {
  if(firstType.equals(secondType)) {
    return firstType;
  } else {
	  TypeInfo result;
	  result.number_of_types_ = firstType.number_of_types_ + secondType.number_of_types_;
	  for(int i = 0; i < kNullType; i++) {
		Type type = (Type)i;
		result.defending_[type] = factorToCode(getFactor(firstType.defending_[type]) * getFactor(secondType.defending_[type]));
	  }
    return result;
  }
}

bool TypeInfo::isValidComposition() const {
  return number_of_types_ == 1 || number_of_types_ == 2;
}

double TypeInfo::getFactor(TypeEffectiveness effectiveness) {
  switch(effectiveness) {
  case kZeroTimes:
	  return 0.0;
	  break;

  case kOneTimes:
	  return 1.0;
	  break;

  case kTwoTimes:
	  return 2.0;
	  break;

  case kFourTimes:
	  return 4.0;
	  break;

  case kHalfTimes:
	  return 0.5;
	  break;

  case kQuarterTimes:
	  return 0.25;
	  break;

  default:
	assert(false);
	return -1.0;
	break;
  }
}

TypeEffectiveness TypeInfo::factorToCode(double factor) {
  if(factor < 0 || factor > 4.0) {
	return kNullTimes;
  } else if (factor == 0.0) {
	return kZeroTimes;
  } else if (factor == 0.25) {
	return kQuarterTimes;
  } else if (factor == 0.5) {
	return kHalfTimes;
  } else if (factor == 1.0) {
	return kOneTimes;
  } else if (factor == 2.0) {
	return kTwoTimes;
  } else if (factor == 4.0) {
	return kFourTimes;
  } else {
	std::cerr << "[Fail] precision error occured" << std::endl;
	assert(false);
	return kNullTimes;
  }
}

bool TypeInfo::equals(const TypeInfo & that) const {
  bool matches = this->number_of_types_ == that.number_of_types_;
  if(!matches) {
	return false;
  } else {
	for(int i = 0; i < kNullType; i++) {
	  if(this->defending_[i] != that.defending_[i])
		  return false;
	}
	return true;
  }
}

TypesHad::TypesHad(Type first_type, Type second_type) : first_type_(first_type), second_type_(second_type) {
  b_dual_type_ = first_type != second_type;
}

TypesHad::TypesHad(Type type) : TypesHad(type, kNullType) {
  b_dual_type_ = false;
}

std::string TypesHad::toString() const {
  std::string string;
  string = resources::getTypeName(first_type_);
  if(b_dual_type_) {
    string = string + "/"s + resources::getTypeName(second_type_); 
  }
  return string;
}

TypeChart::TypeChart() {
	// clear all
	for(int i = 0; i < kNullType; i++) {
		type_set_[i] = false;
		for(int j = 0; j < kNullType; j++) {
			type_info_[i].defending_[j] = kNullTimes;
		}
	}
}

void TypeChart::setType(Type type, TypeInfo info) {
	type_info_[type] = info;
	type_set_[type] = true;
}

bool TypeChart::isComplete() const {
  for(int i = 0; i < kNullType; i++) {
	if(type_set_[i] == false)
		return false;
  }
  return true;
}

std::list<Type> TypeChart::getDefensiveWeaknesses(const TypesHad & types, bool include_1x) const {
	TypeInfo type_info = getTypeInfo(types);
	std::list<Type> weaknesses;
	for(int i = 0; i < kNullType; i++) {
      Type type = (Type)i;
      if(type_info.isWeakAgainst(type) || (include_1x && type_info.defending_[type] == kOneTimes)) {
		weaknesses.push_back(type);
	  }
	}
	return weaknesses;
}

std::list<Type> TypeChart::getDefensiveStrengths(const TypesHad & types, bool include_1x) const {
	TypeInfo type_info = getTypeInfo(types);
	std::list<Type> strengths;
	for(int i = 0; i < kNullType; i++) {
      Type type = (Type)i;
      if(type_info.isStrongAgainst(type) || (include_1x && type_info.defending_[type] == kOneTimes)) {
		strengths.push_back(type);
	  }
	}
	return strengths;
}

std::list<Type> TypeChart::getDefensiveImmunities(const TypesHad & types) const {
	TypeInfo type_info = getTypeInfo(types);
	std::list<Type> immunities;
	for(int i = 0; i < kNullType; i++) {
      Type type = (Type)i;
      if(type_info.defending_[type] == kZeroTimes) {
		immunities.push_back(type);
	  }
	}
	return immunities;
}

std::list<Type> TypeChart::getOffensiveWeaknesses(const TypesHad & types, bool include_1x) const {
  // get weaknesses for first type
  std::list<Type> weaknesses = getOffensiveWeaknessesMonotyped(types.first_type_, include_1x);

  // handle dual type
  if(types.b_dual_type_) {
    // compile weaknesses
    std::list<Type> second_type_is_weak = getOffensiveWeaknessesMonotyped(types.second_type_, include_1x);
    weaknesses.sort();
    second_type_is_weak.sort();
    weaknesses.merge(second_type_is_weak);
    weaknesses.unique();

    // remove strengths
    std::list<Type> strengths = getOffensiveStrengths(types);
    for(Type type : strengths) {
      weaknesses.remove(type);
    }
  }

  // done
  return weaknesses;
}

std::list<Type> TypeChart::getOffensiveStrengths(const TypesHad & types, bool include_1x) const {
  // get first type's strengths
  std::list<Type> strengths = getOffensiveStrengthsMonotyped(types.first_type_, include_1x);

  // add second type's strengths
  if(types.b_dual_type_) {
    std::list<Type> second_type_strengths = getOffensiveStrengthsMonotyped(types.second_type_, include_1x);
    strengths.sort();
    second_type_strengths.sort();
    strengths.merge(second_type_strengths);
    strengths.unique();
  }

  // done
  return strengths;
}

std::list<Type> TypeChart::getOffensiveImmunities(const TypesHad & types) const {
  // get first type's immunities
  std::list<Type> immunities = getOffensiveImmunitiesMonotyped(types.first_type_);

  // add second type's immunities
  if(types.b_dual_type_) {
    std::list<Type> second_type_immunities = getOffensiveImmunitiesMonotyped(types.second_type_);
    immunities.sort();
    second_type_immunities.sort();
    immunities.merge(second_type_immunities);
    immunities.unique();
  }

  // done
  return immunities;
}

bool TypeChart::isWeakTo(const TypesHad & types, Type type) const {
  TypeInfo type_info = getTypeInfo(types);
  return type_info.isWeakAgainst(type);
}

bool TypeChart::isStrongTo(const TypesHad & types, Type type) const {
  TypeInfo type_info = getTypeInfo(types);
  return type_info.isStrongAgainst(type);
}

bool TypeChart::isImmuneTo(const TypesHad & types, Type type) const {
  TypeInfo type_info = getTypeInfo(types);
  return type_info.defending_[type] == kZeroTimes;
}

TypeEffectiveness TypeChart::getTypeEffectivenessXonY(const TypesHad& typingX, const TypesHad& typingY) const {
  if(typingX.b_dual_type_) {
    return maxEffectiveness(getTypeEffectivenessXonY(typingX.first_type_, typingY), getTypeEffectivenessXonY(typingX.second_type_, typingY));
  } else {
    TypeInfo info = getTypeInfo(typingY);
    return info.defending_[typingX.first_type_];
  }
}

TypeEffectiveness TypeChart::maxEffectiveness(const TypeEffectiveness eff1, const TypeEffectiveness eff2) {
  if(eff1 == kNullTimes) {
    return eff2;
  } else if (eff2 == kNullTimes) {
    return eff1;
  } else {
    return eff1 > eff2 ? eff1 : eff2;
  }
}

TypeEffectiveness TypeChart::crossEffectiveness(const TypeEffectiveness x1, const TypeEffectiveness x2) {
  return TypeInfo::factorToCode(TypeInfo::getFactor(x1) * TypeInfo::getFactor(x2));
}

std::list<Type> TypeChart::getOffensiveWeaknessesMonotyped(const Type attacking_type, bool include_1x) const {
  std::list<Type> result;
  for(int i = 0; i < kNullType; i++) {
    Type type = (Type)i;
    if(type_info_[type].isWeakAgainst(attacking_type) || (include_1x && type_info_[type].defending_[attacking_type] == kOneTimes)) {
      result.push_back(type);
    }
  }
  return result;
}

std::list<Type> TypeChart::getOffensiveStrengthsMonotyped(const Type attacking_type, bool include_1x) const {
  std::list<Type> result;
  for(int i = 0; i < kNullType; i++) {
    Type type = (Type)i;
    if(type_info_[type].isStrongAgainst(attacking_type) || (include_1x && type_info_[type].defending_[attacking_type] == kOneTimes)) {
      result.push_back(type);
    }
  }
  return result;
}

std::list<Type> TypeChart::getOffensiveImmunitiesMonotyped(const Type attacking_type) const {
  std::list<Type> result;
  for(int i = 0; i < kNullType; i++) {
    Type type = (Type)i;
    if(type_info_[type].defending_[attacking_type] == kZeroTimes) {
      result.push_back(type);
    }
  }
  return result;
}

TypeInfo TypeChart::getTypeInfo(const TypesHad & typing) const {
	if(!typing.b_dual_type_) {
    assert(typing.first_type_ != kNullType);
		assert(type_set_[typing.first_type_]);
		return type_info_[typing.first_type_];
	} else {
		return getTypeInfo(typing.first_type_, typing.second_type_);
	}
}

TypeInfo TypeChart::getTypeInfo(const Type type1, const Type type2) const {
  assert(type1 != kNullType);
  assert(type2 != kNullType);
	assert(type_set_[type1] && type_set_[type2]);
	return TypeInfo::dualType(type_info_[type1], type_info_[type2]);
}

LearnsetMove::LearnsetMove(const std::string& name) : move_name_(name) {
  learned_at_level_ = 0;
}

void LearnsetMove::setAsMoveTutorableWithMemo(std::string memo) {
  b_tutor_able_ = true;
  tutor_memo_ = memo;
}
Monster::Monster(const std::string & name, const MonsterSpecies * species) : name_(name),
  species_(species) {}

Monster::Monster() : Monster ("???", nullptr) {}

std::string Monster::toString() const {
    std::string species_type;
    std::string species_name;
    if(species_ == nullptr) {
      species_type = "???";
      species_name = "???";
    } else {
      species_name = species_->name_;
      species_type = species_->type_.toString();
    }
    return "'"s + name_ + "'  "s + species_name  + "  "s + species_type;
}

bool typeEffectivenessIsStrong(TypeEffectiveness effectiveness) {
  return effectiveness >= kZeroTimes && effectiveness <= kHalfTimes;
}

bool typeEffectivenessIsWeak(TypeEffectiveness effectiveness) {
  return effectiveness == kTwoTimes || effectiveness == kFourTimes;
}

MonsterSpecies* MonsterSpeciesLibrary::get(const int number) {
  return !numberExists(number) ? nullptr : &number_to_species_.at(number);
}

MonsterSpecies* MonsterSpeciesLibrary::get(const std::string & name) {
  return !nameExists(name) ? nullptr : &number_to_species_.at(name_to_number_.at(name));
}

const MonsterSpecies* MonsterSpeciesLibrary::get(const int number) const {
  return !numberExists(number) ? nullptr : &number_to_species_.at(number);
}

const MonsterSpecies * MonsterSpeciesLibrary::get(const std::string & name) const {
  return !nameExists(name) ? nullptr : &number_to_species_.at(name_to_number_.at(name));
}

void MonsterSpeciesLibrary::set(const int number, const MonsterSpecies & species) {
  number_to_species_[number] = species;
  name_to_number_[species.name_] = number;
}

bool MonsterSpeciesLibrary::numberExists(const int number) const {
  return number_to_species_.find(number) != number_to_species_.end();
}

bool MonsterSpeciesLibrary::nameExists(const std::string & name) const {
  if(name_to_number_.find(name) == name_to_number_.end()) {
    std::cerr << "could not find " << name << std::endl;
    return false;
  } else {
    return numberExists(name_to_number_.at(name));
  }
}


std::vector<std::string> MonsterSpecies::getMovepool() const {
  std::vector<std::string> movepool;
  for(LearnsetMove move : learnset_) {
    movepool.push_back(move.move_name_);
  }
  return movepool;
}

MoveTarget::MoveTarget(MoveTargetDescription description) {
  // who does it target?
  switch(description) {
  case kAnyAdjacentFoe:
  case kAllAdjacentFoes:
  case kAllFoes:
    target_foe = true;
    break;

  case kAnyOther:
  case kAllAdjacent:
  case kAnyAdjacent:
    target_foe = true;
    target_ally = true;
    break;

  case kAll:
    target_self = true;
    target_foe = true;
    target_ally = true;
    break;

  case kSelf:
    target_self = true;
    break;

  case kSelfOrAdjacentAlly:
  case kWholeTeam:
    target_ally = true;
    target_self = true;
    break;

  case kAdjacentAlly:
    target_ally = true;
    break;

  default:
    assert(false);
    break;
  }

  // what is the scope of the targeting?
  switch(description) {
  case kAnyAdjacentFoe:
  case kAllAdjacentFoes:
  case kAllAdjacent:
  case kSelfOrAdjacentAlly:
  case kAdjacentAlly:
  case kAnyAdjacent:
    target_adjacent = true;
    break;

  case kAllFoes:
  case kAnyOther:
  case kAll:
  case kSelf:
  case kWholeTeam:
    target_adjacent = false;
    break;

  default:
    assert(false);
    break;
  }

  // can the target be selected?
  switch(description) {
  case kAnyAdjacentFoe:
  case kAnyOther:
  case kSelfOrAdjacentAlly:
  case kAdjacentAlly:
  case kAnyAdjacent:
    target_any = true;
    break;

  case kAllAdjacentFoes:
  case kAllFoes:
  case kAllAdjacent:
  case kAll:
  case kSelf:
  case kWholeTeam:
    target_any = false;
    break;

  default:
    assert(false);
    break;
  }
}

MonsterStats::MonsterStats() {
  hp_ = attack_ = defense_ = special_attack_ = special_defense_ = speed_ = 0;
}

bool MonsterStats::isBlank() const {
  const int* stats[6] = {&hp_, &attack_, &defense_, &special_attack_, &special_defense_, &speed_};
  long long total = 0;
  for(int i = 0; i < 6; i++) {
    total += std::abs((long long)*stats[i]);
  }
  return total == 0;
}

} //namespace pokeman