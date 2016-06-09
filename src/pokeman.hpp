/*
* pokeman.hpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
* 
* Classes for the pokeman project.
*/

#ifndef POKEMAN_POKEMAN_HPP_
#define POKEMAN_POKEMAN_HPP_

#include <bitset>
#include <list>
#include <map>
#include <string>
#include <vector>

#define POKEMAN_NUMBER_OF_GENERATIONS_TO_CAP 8

namespace pokeman {

/// Enumeration of types
enum Type {
  kNormal,
  kFighting,
  kFlying,
  kPoison,
  kGround,
  kRock,
  kBug,
  kGhost,
  kSteel,
  kFire,
  kWater,
  kGrass,
  kElectric,
  kPsychic,
  kIce,
  kDragon,
  kDark,
  kFairy,
  kNullType
};

/// Move types.
enum MoveType {
  kPhysical,
  kSpecial,
  kStatus,
  kNullMoveType
};

/// Move scopes
enum MoveTargetDescription {
  // Triple Battles
  kAnyAdjacentFoe,
  kAllAdjacentFoes,
  kAllFoes,
  kAnyOther,
  kAllAdjacent,
  kAll,
  kSelf,
  kSelfOrAdjacentAlly,
  kAdjacentAlly,
  kWholeTeam,
  kAnyAdjacent,
  
  // Null
  kNullTargetDescription

};

/// Strength of move
enum TypeEffectiveness {
  kZeroTimes,
  kQuarterTimes,
  kHalfTimes,
  kOneTimes,
  kTwoTimes,
  kFourTimes,
  kNullTimes
};

bool typeEffectivenessIsStrong(TypeEffectiveness effectiveness);

bool typeEffectivenessIsWeak(TypeEffectiveness effectiveness);

/// Holds type info
class TypeInfo {
public:
  /// holds base 2 exponents denoting the factor of damage taken while defending by this type.
  TypeEffectiveness defending_[kNullType];

private:
  /// 1 indicates a monotype, 2 indicates a dual type.
  int number_of_types_;

public:
  /// Constructor, initialization of values.
  TypeInfo();

  /// checks whether the damage multiplier is greater than 1x against given type
  bool isWeakAgainst(Type type) const;

  /// checks whether the damage multiplier is a fraction of 1x against given type
  bool isStrongAgainst(Type type) const;

  /// returns the result of dual-typing this and another type.
  static TypeInfo dualType(TypeInfo firstType, TypeInfo secondType);

  /// false, if this does not represent a mono or dual type.
  bool isValidComposition() const;

  /// result of 2^exponent
  static double getFactor(TypeEffectiveness effectiveness);

  /// turns a float into effectiveness
  static TypeEffectiveness factorToCode(double factor);

private:
  /// Checks if two types are the same.
  bool equals(const TypeInfo& that) const;
};

/// Holds a pokemon type
class TypesHad {
public:
  /// Primary typing of pokemon
  Type first_type_;

  /// Subtype of pokemon
  Type second_type_;

  /// Indicates whether the second type is junk or not
  bool b_dual_type_;

  /// Dual-type constructor
  TypesHad(Type first_type, Type second_type);

  /// Monotyped constructor
  TypesHad(Type type = kNullType);

  std::string toString() const;
};



/// Lookup Type Info across all types.
class TypeChart {
private:
  /// Type charts for all types, including dual types.
  /// An entry like type_info_[x][x] represents the mono type.
  TypeInfo type_info_[kNullType];

  /// Keeps track of which types have been set.
  /// False values indicate that a type info has not yet been described.
  bool type_set_[kNullType];

public:
  /// flag set to TRUE if the last 'get' function keyed an unset type.
  bool invalid_lookup_;

public:
  /// Initializes a blank type chart.
  TypeChart();

  /// Adds type to chart. Causes all set types to be updated.
  void setType(Type type, TypeInfo info);

  /// Checks if all types have been set.
  bool isComplete() const;

  /// Checks which types are defended against poorly.
  std::list<Type> getDefensiveWeaknesses(const TypesHad& types, bool include_1x = false) const;

  /// Checks which types are defended against well.
  std::list<Type> getDefensiveStrengths(const TypesHad& types, bool include_1x = false) const;

  /// Checks which types will cause no damage whatsoever.
  std::list<Type> getDefensiveImmunities(const TypesHad& types) const;

  /// Checks which types this will be supereffective against.
  std::list<Type> getOffensiveWeaknesses(const TypesHad& types, bool include_1x = false) const;

  /// Checks which types this will do less damage against.
  std::list<Type> getOffensiveStrengths(const TypesHad& types, bool include_1x = false) const;

  /// Checks which types this will do no damage against.
  std::list<Type> getOffensiveImmunities(const TypesHad& types) const;

  /// Checks if this type combination is weak to an element.
  bool isWeakTo(const TypesHad& types, Type type) const;

  /// Checks if this type combination is strong to an element.
  bool isStrongTo(const TypesHad& types, Type type) const;

  /// Checks if this type combination is immune to an element.
  bool isImmuneTo(const TypesHad& types, Type type) const;

  /// Get type effectiveness of type X against type Y
  TypeEffectiveness getTypeEffectivenessXonY(const TypesHad& typingX, const TypesHad& typingY) const;

private:
  static TypeEffectiveness maxEffectiveness(const TypeEffectiveness eff1, const TypeEffectiveness eff2);

  static TypeEffectiveness crossEffectiveness(const TypeEffectiveness x1, const TypeEffectiveness x2);

  std::list<Type> getOffensiveWeaknessesMonotyped(const Type attacking_type, bool include_1x) const;

  std::list<Type> getOffensiveStrengthsMonotyped(const Type attacking_type, bool include_1x) const;

  std::list<Type> getOffensiveImmunitiesMonotyped(const Type attacking_type) const;

  TypeInfo getTypeInfo(const TypesHad& typing) const;

  TypeInfo getTypeInfo(const Type type1, const Type type2) const;
};

/// Describes the target of a move.
class MoveTarget {
public:
  /// targets foe
  bool target_foe;

  /// targets ally
  bool target_ally;

  /// targets self
  bool target_self;
  
  /// When true, the target can be chosen among valid targets.
  bool target_any;

  /// When false, the position of the pokemon does not make a difference.
  bool target_adjacent;

  /// Constructs based on description.
  MoveTarget(MoveTargetDescription description);

};

class MonsterStats {
public:
  /// Monster's stats
  int hp_, attack_, defense_, special_attack_, special_defense_, speed_;

  /// Constructor
  MonsterStats();

  /// True, if all entries are 0
  bool isBlank() const;
};

/// Holds data for a battle move.
class Move {
public:
  /// Name of the move.
  std::string name_;

  /// Attack classification.
  MoveType move_type_;

  /// Elemental classification
  Type elemental_type_;

  /// Power of the move.
  int power_;

  /// Accuracy of the move.
  int accuracy_;

  /// Chance of inflicting status effect
  int status_effect_chance_self_;
  int status_effect_change_target_;

  /// Modifiers of stats
  MonsterStats stat_modifiers_self_;

  /// Modifiers of stats
  MonsterStats stat_modifiers_target_;

  /// HM number. (0 is no hm)
  int hm_;

  /// TM number. (0 is no tm)
  int tm_;

  /// Move target
  MoveTargetDescription target_;

  /// Generations for which this move is valid.
  std::bitset<POKEMAN_NUMBER_OF_GENERATIONS_TO_CAP> valid_generations_;

  /// PP of move
  int pp_;

  /// Index of move
  int number_;
};

/// Entry in a pokemon's learnset.
class LearnsetMove {
public:
  /// Name of the move to be learned.
  std::string move_name_;

  /// Can be learned through HM or TM?
  bool b_machine_able_;

  /// Can be learned through move tutor?
  bool b_tutor_able_;

  /// Memo for move tutor.
  std::string tutor_memo_;

  /// Which level is this learned at? (0 means it is not learned)
  int learned_at_level_;

  /// Blank constructor.
  LearnsetMove(const std::string& name = "???");

  /// Set as a move tutorable move.
  void setAsMoveTutorableWithMemo(std::string memo);
};

/// Holds data for a pokemon's species.
class MonsterSpecies {
public:
  /// What the species is called.
  std::string name_;

  /// What type the species is.
  TypesHad type_;

  /// Learnset
  std::vector<LearnsetMove> learnset_;

  /// Base stats
  MonsterStats base_stats_;

  /// What generation(s) this applies to
  std::bitset<POKEMAN_NUMBER_OF_GENERATIONS_TO_CAP> valid_generations;

  /// retrieves movepool
  std::vector<std::string> getMovepool() const;
};

class MonsterSpeciesLibrary {
private:
  /// Maps species name to dex number
  std::map<std::string, int> name_to_number_;

  /// Maps dex number to species
  std::map<int, MonsterSpecies> number_to_species_;
public:
  /// Retrieve species by pokedex number
  MonsterSpecies* get(const int number);
  
  /// Retrieve species by name
  MonsterSpecies* get(const std::string& name);

  /// Retrieve species by pokedex number
  const MonsterSpecies* get(const int number) const;
  
  /// Retrieve species by name
  const MonsterSpecies* get(const std::string& name) const;

  /// Add an entry
  void set(const int number, const MonsterSpecies& species);

private:
  /// True, if dex number has an entry.
  bool numberExists(const int number) const;

  /// True, if species name has an entry.
  bool nameExists(const std::string& name) const;

};

/// Holds data for a pokemon.
class Monster {
public:
  /// Nickname of the monster.
  std::string name_;

  /// Species of the monster.
  const MonsterSpecies* species_;

  /// HM/ moves to keep
  std::vector<std::string> hm_moves_;

public:
  /// Construct with nickname & species/
  Monster(const std::string& name, const MonsterSpecies* species);

  /// Construct as blank.
  Monster();

  std::string toString() const;
};

} //namespace pokeman

#endif //POKEMAN_POKEMAN_HPP_