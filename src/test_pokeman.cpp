/*
* test_pokeman.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/

#include <cassert>

#include <iostream>

#include "test_pokeman.hpp"
#include "pokeman.hpp"

#include "resources.hpp"

#define TEST_POKEMAN_NUMBER_OF_TYPES 18

namespace pokeman {
namespace test {

// lookup the type string
const char* getTypeString(Type type);

// lookup the type effectiveness string
const char* getTypeEffectivenessString(TypeEffectiveness type);

// type tests
int test_pokeman_type_count();

// type info tests
int test_type_info_init();
int test_pokeman_type_info_dual_type();
int test_pokeman_type_info_dual_type_triple_type();
int test_pokeman_type_info_is_against();
int test_pokeman_type_info_get_factor();

// types had tests
int test_pokeman_types_had();

// typechart tests
int test_type_chart_set_type();
int test_type_chart_is_complete();
int test_type_chart_lookup_correctness();
int test_type_chart_quick();
int test_type_chart_xy();

// learnset move tests
int test_learnset_move_memo();

// test suite
static const TestNode test_pokeman_tests[] = {
  {"type count", "checks if all types have been included.", test_pokeman_type_count},
  { "init type info", "checks if type info is initialized properly.", test_type_info_init},
  {"dual type", "checks if dual typing works correctly.", test_pokeman_type_info_dual_type},
  {"triple type", "checks if triple types are identified as invalid.", test_pokeman_type_info_dual_type_triple_type},
  {"booleanize strong/weak", "checks 'isWeakAgainst' & 'isStrongAgainst'", test_pokeman_type_info_is_against},
  {"get factor", "checks if factors are calculated correctly.", test_pokeman_type_info_get_factor},
  {"types had", "checks if TypesHad object is constructed with data.", test_pokeman_types_had},
  {"set chart type", "checks if type is set with correct effects.", test_type_chart_set_type},
  {"chart completeness", "checks if chart shows it is complete when appropriate", test_type_chart_is_complete},
  {"chart lookup correctness", "checks if weaknesses, strengths, & immunities are looked up correctly.", test_type_chart_lookup_correctness},
  {"check single lookup", "checsk type chart isWeakTo, etc.", test_type_chart_quick},
  {"check effectiveness lookup", "checks if XY function works", test_type_chart_xy},
  {"learnset move memo", "checks that memo is set when function is called", test_learnset_move_memo},
  {nullptr, nullptr, NULL}
};

const TestNode* pokeman_suite() {
  return test_pokeman_tests;
}

// type strings
static const NameEnumHarness type_strings[] = {
  {kNormal, "Normal"},
  {kFighting, "Fighting"},
  {kFlying, "Flying"},
  {kPoison, "Poison"},
  {kGround, "Ground"},
  {kRock, "Rock"},
  {kBug, "Bug"},
  {kGhost, "Ghost"},
  {kSteel, "Steel"},
  {kFire, "Fire"},
  {kWater, "Water"},
  {kElectric, "Electric"},
  {kPsychic, "Psychic"},
  {kIce, "Ice"},
  {kDragon, "Dragon"},
  {kDark, "Dark"},
  {kFairy, "Fairy"},
  {kNullType, "UNKNOWN TYPE"},
};

const char* getTypeString(Type type) {
  return getName(type_strings, (int)type, kNullType);
}

static const NameEnumHarness type_effectiveness_strings[] = {
  {kZeroTimes, "0x"},
  {kOneTimes, "1x"},
  {kTwoTimes, "2x"},
  {kFourTimes, "4x"},
  {kHalfTimes, "0.5x"},
  {kQuarterTimes, "0.25x"},
  {kNullTimes, "??x"}
};

const char * getTypeEffectivenessString(TypeEffectiveness type) {
  return getName(type_effectiveness_strings, (int)type, kNullTimes);
}

int test_pokeman_type_count() {
  return kNullType == TEST_POKEMAN_NUMBER_OF_TYPES ? 0 : 1;
}

int test_type_info_init() {
  // here it is
  TypeInfo info;

  // check factors
  for(int i = 0; i < kNullType; i++) {
    if(info.defending_[i] != kOneTimes) {
      std::cerr << "[Error]: '" << getTypeString((Type)i)
          << "' was initialized to " << getTypeEffectivenessString(info.defending_[i])
          << " instead of " << getTypeEffectivenessString(kOneTimes) << std::endl;
      return 1;
    }
  }

  // done
  return 0;
}

int test_pokeman_type_info_dual_type() {
  // outset
  TypeInfo infoA, infoB, infoAB_expected;

  // A and AB are strong against normal
  infoA.defending_[kNormal] = kHalfTimes;
  infoB.defending_[kNormal] = kOneTimes;
  infoAB_expected.defending_[kNormal] = kHalfTimes;

  // B and AB are weak against fighting
  infoA.defending_[kFighting] = kOneTimes;
  infoB.defending_[kFighting] = kTwoTimes;
  infoAB_expected.defending_[kFighting] = kTwoTimes;

  // All are chill about flying
  infoA.defending_[kFlying] = kOneTimes;
  infoB.defending_[kFlying] = kOneTimes;
  infoAB_expected.defending_[kFlying] = kOneTimes;

  // AB is not weak to Poison, though B is
  infoA.defending_[kPoison] = kHalfTimes;
  infoB.defending_[kPoison] = kTwoTimes;
  infoAB_expected.defending_[kPoison] = kOneTimes;

  // AB is doubly strong against ground
  infoA.defending_[kGround] = kHalfTimes;
  infoB.defending_[kGround] = kHalfTimes;
  infoAB_expected.defending_[kGround] = kQuarterTimes;

  // Though AB is doubly weak to rock. Ow.
  infoA.defending_[kRock] = kTwoTimes;
  infoB.defending_[kRock] = kTwoTimes;
  infoAB_expected.defending_[kRock] = kFourTimes;
  
  // All are immune to bug.
  infoA.defending_[kBug] = kZeroTimes;
  infoB.defending_[kBug] = kZeroTimes;
  infoAB_expected.defending_[kBug] = kZeroTimes;

  // A & AB are immune to ghost.
  infoA.defending_[kGhost] = kZeroTimes;
  infoB.defending_[kGhost] = kOneTimes;
  infoAB_expected.defending_[kGhost] = kZeroTimes;

  // B & AB are immune to steel.
  infoA.defending_[kSteel] = kOneTimes;
  infoB.defending_[kSteel] = kZeroTimes;
  infoAB_expected.defending_[kSteel] = kZeroTimes;

  // Get two actuals, compare them for equality
  TypeInfo infoAB_actual = TypeInfo::dualType(infoA, infoB);
  TypeInfo infoAB_actual_2 = TypeInfo::dualType(infoB, infoA);
  for(int i = 0; i < kNullType; i++) {
    if(infoAB_actual.defending_[i] != infoAB_actual_2.defending_[i]) {
      std::cerr << "Results of 'dualType' were not found to be commutative!" << std::endl
                << "  Reason: type " << getTypeString((Type)i) << "was found to be "
                << getTypeEffectivenessString(infoAB_actual.defending_[i]) << " & "
                << getTypeEffectivenessString(infoAB_actual_2.defending_[i])
                << std::endl;
      return 100;
    }
  }

  // Compare actual and expected, now.
  for(int i = 0; i < kNullType; i++) {
    TypeEffectiveness expected = infoAB_expected.defending_[i];
    TypeEffectiveness actual = infoAB_actual.defending_[i];
    if(expected != actual) {
      std::cerr << "Results of 'dualType' were not found to be accurate!"
                << std::endl << "  Reason: type " << getTypeString((Type)i)
                << " was found to be " << getTypeEffectivenessString(actual)
                << " instead of " << getTypeEffectivenessString(expected)
                << std::endl;
      return 200;
    }
  }

  // I guess it worked out OK
  return 0;
}

int test_pokeman_type_info_dual_type_triple_type() {
  // mono type
  TypeInfo typeA;
  typeA.defending_[0] = kZeroTimes;
  if(!typeA.isValidComposition()) {
    std::cerr << "[Error] Monotype was declared invalid composition" << std::endl;
    return 300;
  }

  // dual type
  TypeInfo typeB;
  typeA.defending_[1] = kZeroTimes;
  TypeInfo typeAB = TypeInfo::dualType(typeA, typeB);
  if(!typeAB.isValidComposition()) {
    std::cout << "[Error] Dual type was declared invalid composition" << std::endl;
    return 310;
  }

  // (invalid) triple type
  TypeInfo typeABA = TypeInfo::dualType(typeAB, typeA);
  if(typeABA.isValidComposition()) {
    std::cout << "[Error] triple type was declared valid composition" << std::endl;
    return 320;
  }

  // done
  return 0;
}

int test_pokeman_type_info_is_against() {
  // initialize type
  TypeInfo type_info;
  for(int i = 0; i < kNullTimes; i++) {
    type_info.defending_[kNullType - (i + 1)] = (TypeEffectiveness)i;
  }

  // expected
  bool expected_strength[kNullTimes];
  expected_strength[kOneTimes] = false;
  expected_strength[kTwoTimes] = false;
  expected_strength[kFourTimes] = false;
  expected_strength[kZeroTimes] = true;
  expected_strength[kHalfTimes] = true;
  expected_strength[kQuarterTimes] = true;
  bool expected_weakness[kNullTimes];
  expected_weakness[kOneTimes] = false;
  expected_weakness[kTwoTimes] = true;
  expected_weakness[kFourTimes] = true;
  expected_weakness[kZeroTimes] = false;
  expected_weakness[kHalfTimes] = false;
  expected_weakness[kQuarterTimes] = false;

  // check 'em
  for(int i = 0; i < kNullTimes; i++) {
    // current type
    Type type = (Type)(kNullType - (i + 1));

    // check weakness rating
    bool is_weak_expected = expected_weakness[i];
    bool is_weak_actual = type_info.isWeakAgainst(type);
    if(is_weak_expected != is_weak_actual) {
      std::cout << "[Error] element " << getTypeString(type) << " with strength "
        << getTypeEffectivenessString((TypeEffectiveness)i)
        << " was found to be " << is_weak_actual << " weak" << std::endl;
      return 400;
    }

    // check strength rating
    bool is_strong_expected = expected_strength[i];
    bool is_strong_actual = type_info.isStrongAgainst(type);
    if (is_strong_expected != is_strong_actual) {
      std::cout << "[Error] element " << getTypeString(type) << " with strength "
        << getTypeEffectivenessString((TypeEffectiveness)i)
        << " was found to be " << is_strong_actual << " strong" << std::endl;
      return 410;
    }
  }

  return 0;
}

int test_pokeman_type_info_get_factor() {
  double expected[kNullTimes];
  expected[kZeroTimes] = 0.0;
  expected[kOneTimes] = 1.0;
  expected[kTwoTimes] = 2.0;
  expected[kFourTimes] = 4.0;
  expected[kHalfTimes] = 0.5;
  expected[kQuarterTimes] = 0.25;
  for(int i = 0; i < kNullTimes; i++) {
    double actual_value = TypeInfo::getFactor((TypeEffectiveness)i);
    if(expected[i] != actual_value) {
      std::cerr << "[Error] For "
        << getTypeEffectivenessString((TypeEffectiveness)i)
        << ", expected " << expected[i] << " but got "
        << actual_value << std::endl;
      return 500;
    }
  }
  return 0;
}

int test_pokeman_types_had() {
  // test monotype 1
  TypesHad monotype(kNormal);
  if(monotype.b_dual_type_ == true) {
    std::cerr << "[Error] Monotype was treated as dual type" << std::endl;
    return 600;
  } else if (monotype.first_type_ != kNormal) {
    std::cerr << "[Error] Monotype was set to " << getTypeString(monotype.first_type_) << ", not " << getTypeString(kNormal) << std::endl;
    return 610;
  }

  // test monotype 2
  TypesHad monotype_2(kNormal, kNormal);
  if (monotype_2.b_dual_type_ == true) {
    std::cerr << "[Error] Second Monotype was treated as dual type" << std::endl;
    return 620;
  } else if (monotype_2.first_type_ != kNormal) {
    std::cerr << "[Error] Second Monotype was set to " << getTypeString(monotype_2.first_type_) << ", not " << getTypeString(kNormal) << std::endl;
    return 630;
  }

  // test dual type
  TypesHad dualtype(kNormal, kFighting);
  if (dualtype.b_dual_type_ == false) {
    std::cerr << "[Error] Dual type was treated as mono type" << std::endl;
    return 640;
  } else if (dualtype.first_type_ != kNormal || dualtype.second_type_ != kFighting) {
    std::cerr << "[Error] Dual type was set to " << getTypeString(dualtype.first_type_) << "/" << getTypeString(dualtype.second_type_)
      << ", not Normal/Fighting" << std::endl;

    return 650;
  }

  // we're good
  return 0;
}

int test_type_chart_set_type() {
  // setup normal type
  TypeInfo normal;
  normal.defending_[kFighting] = kTwoTimes;
  normal.defending_[kGhost] = kZeroTimes;

  // setup dark type
  TypeInfo dark;
  dark.defending_[kFighting] = kTwoTimes;
  dark.defending_[kBug] = kTwoTimes;
  dark.defending_[kGhost] = kHalfTimes;
  dark.defending_[kPsychic] = kZeroTimes;
  dark.defending_[kDark] = kHalfTimes;
  dark.defending_[kFairy] = kTwoTimes;

  // setup chart
  TypeChart chart;
  chart.setType(kNormal, normal);
  chart.setType(kDark, dark);

  // check immunities
  std::list<Type> immunities = chart.getDefensiveImmunities(kNormal);
  if(immunities.empty() || immunities.size() > 1 || immunities.front() != kGhost) {
    std::cerr << "[Error] Found " << immunities.size() << " immunities instead of 1";
    if(!immunities.empty()) {
      std::cerr << ", first was " << getTypeString(immunities.front());
    }
    std::cerr << std::endl;
    return 700;
  }

  // check weaknesses
  std::list<Type> weaknesses = chart.getDefensiveWeaknesses(kNormal);
  if (weaknesses.empty() || weaknesses.size() > 1 || weaknesses.front() != kFighting) {
    std::cerr << "[Error] Found " << weaknesses.size() << " weaknesses instead of 1";
    if (!weaknesses.empty()) {
      std::cerr << ", first was " << getTypeString(weaknesses.front());
    }
    std::cerr << std::endl;
    return 710;
  }

  // check strengths
  std::list<Type> strengths = chart.getDefensiveStrengths(kNormal);
  if (strengths.empty() || strengths.size() > 1 || strengths.front() != kGhost) {
    std::cerr << "[Error] Found " << strengths.size() << " strengths instead of 1";
    if (!strengths.empty()) {
      std::cerr << ", first was " << getTypeString(strengths.front());
    }
    std::cerr << std::endl;
    return 720;
  }

  // check dark
  int dark_immunity_count = chart.getDefensiveImmunities(kDark).size();
  int dark_strength_count = chart.getDefensiveStrengths(kDark).size();
  int dark_weakness_count = chart.getDefensiveWeaknesses(kDark).size();
  if (dark_immunity_count != 1) {
    std::cerr << "Found " << dark_immunity_count << " immunities, not 1" << std::endl;
    return 730;
  } else if (dark_strength_count != 3) {
    std::cerr << "Found " << dark_strength_count << " strengths, not 3" << std::endl;
    return 740;
  } else if (dark_weakness_count != 3) {
    std::cerr << "Found " << dark_weakness_count << " weaknesses, not 3" << std::endl;
    return 750;
  }

  // we're good
  return 0;
}

int test_type_chart_is_complete() {
  // start
  TypeChart chart;
  
  // check empty
  if(chart.isComplete()) {
    std::cerr << "[Error] chart appears complete on initalization" << std::endl;
    return 800;
  }

  // check when set up to dark
  for(int i = 0; i < (kNullType - 1); i++) {
    chart.setType((Type)i, TypeInfo());
    if(chart.isComplete()) {
      std::cerr << "[Error] chart showed as complete when set up to "
        << getTypeString((Type)i) << std::endl;
      return 810;
    }
  }

  // check when actually filled
  chart.setType((Type)(kNullType - 1), TypeInfo());
  if(!chart.isComplete()) {
    std::cerr << "[Error] chart did NOT show as complete when filled" << std::endl;
    return 820;
  }

  // finish!
  return 0;
}

int test_type_chart_lookup_correctness() {
  // Flying type
  TypeInfo flying;
  flying.defending_[kFighting] = kHalfTimes;
  flying.defending_[kGround] = kZeroTimes;
  flying.defending_[kRock] = kTwoTimes;
  flying.defending_[kBug] = kHalfTimes;
  flying.defending_[kGrass] = kHalfTimes;
  flying.defending_[kElectric] = kTwoTimes;
  flying.defending_[kIce] = kTwoTimes;

  // Water type
  TypeInfo water;
  water.defending_[kSteel] = kHalfTimes;
  water.defending_[kFire] = kHalfTimes;
  water.defending_[kWater] = kHalfTimes;
  water.defending_[kGrass] = kTwoTimes;
  water.defending_[kElectric] = kTwoTimes;
  water.defending_[kIce] = kHalfTimes;

  // check weaknesses of water/flying
  TypeChart chart;
  chart.setType(kFlying, flying);
  chart.setType(kWater, water);

  // get info
  TypesHad blue_man(kFlying, kWater);
  std::list<Type> strengths = chart.getDefensiveStrengths(blue_man);
  std::list<Type> weaknesses = chart.getDefensiveWeaknesses(blue_man);
  std::list<Type> immunities = chart.getDefensiveImmunities(blue_man);

  // check immunities
  if(immunities.empty() || immunities.front() != kGround) {
    std::cout << "immunities was found to be of size " << immunities.size() << std::endl;
    return 900;
  }

  // check weaknesses
  if(weaknesses.size() != 2) {
    std::cout << "number of weaknesses was found to be " << weaknesses.size() << std::endl;
    return 910;
  }

  // done
  return 0;
}

int test_type_chart_quick() {
  // construct types
  TypeInfo typeA_info, typeB_info;
  typeA_info.defending_[kNormal] = kHalfTimes;
  typeB_info.defending_[kNormal] = kTwoTimes;
  typeB_info.defending_[kFighting] = kZeroTimes;

  // Make type chart
  TypeChart chart;
  Type typeA_type = (Type)0;
  Type typeB_type = (Type)1;
  chart.setType(typeA_type, typeA_info);
  chart.setType(typeB_type, typeB_info);

  // Get Data
  TypesHad typesHadAB(typeA_type, typeB_type);
  bool typeA_strongToNormal = chart.isStrongTo(typeA_type, kNormal);
  bool typeB_weakToNormal = chart.isWeakTo(typeB_type, kNormal);
  bool typeAB_strongToNormal = chart.isStrongTo(typesHadAB, kNormal);
  bool typeAB_weakToNormal = chart.isStrongTo(typesHadAB, kNormal);
  bool typeB_strongToFighting = chart.isStrongTo(typeB_type, kFighting);
  bool typeB_immuneToFighting = chart.isImmuneTo(typeB_type, kFighting);
  bool typeAB_strongToFighting = chart.isStrongTo(typesHadAB, kFighting);
  bool typeAB_immuneToFighting = chart.isImmuneTo(typesHadAB, kFighting);
  
  // try data
  if(!typeA_strongToNormal
      || !typeB_weakToNormal
      || typeAB_strongToNormal
      || typeAB_weakToNormal
      || !typeB_strongToFighting
      || !typeB_immuneToFighting
      || !typeAB_strongToFighting
      || !typeAB_immuneToFighting) {
    std::cerr << "[Error] 'is' functions not correct: " << typeA_strongToNormal
      << typeB_weakToNormal << typeAB_strongToNormal << typeAB_weakToNormal
      << typeB_strongToFighting << typeB_immuneToFighting << typeAB_strongToFighting
      << typeAB_immuneToFighting << std::endl;
    return 950;
  }

  // Done
  return 0;
}

int test_type_chart_xy() {
  Type typeA, typeB, typeC;
  typeA = (Type)0;
  typeB = (Type)1;
  typeC = (Type)2;

  TypeInfo typeA_info, typeB_info, typeC_info;
  typeA_info.defending_[typeC] = kTwoTimes;
  typeB_info.defending_[typeA] = kTwoTimes;
  typeB_info.defending_[typeC] = kHalfTimes;
  typeC_info.defending_[typeA] = kTwoTimes;
  
  TypeChart chart;
  chart.setType(typeA, typeA_info);
  chart.setType(typeB, typeB_info);
  chart.setType(typeC, typeC_info);

  TypeEffectiveness b_v_a = chart.getTypeEffectivenessXonY(typeB, typeA);
  TypeEffectiveness a_v_b = chart.getTypeEffectivenessXonY(typeA, typeB);
  TypeEffectiveness a_v_c = chart.getTypeEffectivenessXonY(typeA, typeC);
  TypeEffectiveness c_v_a = chart.getTypeEffectivenessXonY(typeC, typeA);
  TypeEffectiveness bc_v_a = chart.getTypeEffectivenessXonY(TypesHad(typeB, typeC), typeA);
  TypeEffectiveness a_v_bc = chart.getTypeEffectivenessXonY(typeA,TypesHad(typeB, typeC));
  TypeEffectiveness ab_v_c = chart.getTypeEffectivenessXonY(TypesHad(typeA, typeB), typeC);
  TypeEffectiveness c_v_ab = chart.getTypeEffectivenessXonY(typeC,TypesHad(typeA, typeB));

  std::vector<TypeEffectiveness> actual =  {b_v_a, a_v_b, a_v_c, c_v_a, bc_v_a, a_v_bc, ab_v_c, c_v_ab};
  std::vector<TypeEffectiveness> expected = {kOneTimes, kTwoTimes, kTwoTimes, kTwoTimes, kTwoTimes, kFourTimes, kTwoTimes, kOneTimes};
  std::vector<std::string> description = { "B vs A", "A vs B", "A vs C", "C vs A", "BC vs A", "A vs BC", "AB vs C", "C vs AB"};
  for(int i = 0; i < (int)actual.size(); i++) {
    if(actual[i] != expected[i]) {
      std::cout << "[Fail] on " << description[i] << std::endl << "  Expected: " << resources::getTypeEffectivenessDescription(expected[i])
        << std::endl << "  Actual: " << resources::getTypeEffectivenessDescription(actual[i]) << std::endl;
      return 100;
    }
  }

  return 0;
}

int test_learnset_move_memo() {
  LearnsetMove learnset_move("test");
  learnset_move.setAsMoveTutorableWithMemo("test memo");
  if(learnset_move.tutor_memo_.compare("test memo") != 0) {
    std::cout << "test memo was not set correctly" << std::endl;
    return 1000;
  }
  return 0;
}

} // namespace test
} // namespace pokeman