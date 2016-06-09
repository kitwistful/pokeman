/*
* test_resources.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/
#include "test_resources.hpp"

#include <iostream>

#include "resources.hpp"

namespace pokeman {
namespace test {

int check_types_all_exist();
int check_gen5_chart();

static const TestNode test_resource_tests[] = {
  { "check types all exist", "ensures all types have an entry", check_types_all_exist},
  { "gen5 chart", "ensures gen5 chart is valid", check_gen5_chart},
  { nullptr, nullptr, NULL }
};

const TestNode* resource_suite() {
  return test_resource_tests;
}

std::string getTypesHadString(const TypesHad& types_had) {
  if(!types_had.b_dual_type_) {
    return resources::getTypeName(types_had.first_type_);
  } else {
    return resources::getTypeName(types_had.first_type_) + std::string("/")
      + resources::getTypeName(types_had.second_type_);
  }
}

int check_types_all_exist()
{
  for(int i = 0; i < kNullType; i++) {
    std::string name = resources::getTypeName((Type)i);
    if(name.compare("NULL") == 0) {
      std::cerr << "type " << i << " was unset" << std::endl;
      return 100;
    }
  }
  return 0;
}

struct ChartCheckNode {
  TypesHad types;
  Type typeToCheck;
  bool isWeak;
  bool isStrong;
  bool isImmune;
};

int doChartCheckNode(const TypeChart& chart, const ChartCheckNode node) {
  bool isImmune = chart.isImmuneTo(node.types, node.typeToCheck);
  bool isWeak = chart.isWeakTo(node.types, node.typeToCheck);
  bool isStrong = chart.isStrongTo(node.types, node.typeToCheck);
  if(node.isImmune != isImmune) {
    std::cerr << "[Error] immunity error on " << getTypesHadString(node.types) << " v. " 
      << resources::getTypeName(node.typeToCheck) << std::endl;
    return -1;
  } else if (node.isImmune && !isStrong) {
    std::cerr << "[Error] strength C immunity error on " << getTypesHadString(node.types) << " v. "
      << resources::getTypeName(node.typeToCheck) << std::endl;
    return -5;
  } else if (!node.isImmune && (node.isStrong != isStrong)) {
    std::cerr << "[Error] strength error on " << getTypesHadString(node.types) << " v. "
      << resources::getTypeName(node.typeToCheck) << std::endl;
    return -10;
  } else if (node.isWeak != isWeak) {
    std::cerr << "[Error] weakness error on " << getTypesHadString(node.types) << " v. "
      << resources::getTypeName(node.typeToCheck) << std::endl;
    return -15;
  } else {
    return 0;
  }
}

int check_gen5_chart() {
  TypeChart chart = resources::generateTypeChartGen5();
  chart.setType(kFairy, TypeInfo());
  if(!chart.isComplete()) {
    std::cout << "not all elements are set" << std::endl;
    return 200;
  }

  // my pokemans
  TypesHad buster_type(kFire, kFighting);
  TypesHad salad_type(kBug, kGrass);
  TypesHad col_type(kBug, kRock);
  TypesHad blue_type(kWater, kFlying);
  TypesHad lady_type(kGround, kDark);

  // stuff to check
  const ChartCheckNode tests[] = {
    // monotype
    {kNormal, kFighting, true, false, false},
    {kNormal, kGhost, false, true, true},
    {kFighting, kBug, false, true, false},
    {kFighting, kDark, false, true, false},
    {kFighting, kRock, false, true, false},
    {kFighting, kFlying, true, false, false},
    {kFighting, kPsychic, true, false, false},
    {kFlying, kBug, false, true, false},
    {kFlying, kFighting, false, true, false},
    {kFlying, kGrass, false, true, false},
    {kFlying, kElectric, true, false, false},
    {kFlying, kIce, true, false, false},
    {kFlying, kRock, true, false, false},
    {kFlying, kGround, false, true, true},

    // my dual-type pokemans
    {buster_type, kFlying, true, false, false},
    {buster_type, kGround, true, false, false},
    {buster_type, kWater, true, false, false},
    {salad_type, kPoison, true, false, false},
    {salad_type, kRock, true, false, false},
    {salad_type, kBug, true, false, false},
    {salad_type, kIce, true, false, false},
    {salad_type, kFlying, true, false, false},
    {salad_type, kFire, true, false, false},
    {lady_type, kBug, true, false, false},
    {lady_type, kWater, true, false, false},
    {lady_type, kGrass, true, false, false},
    {lady_type, kIce, true, false, false},
    {blue_type, kRock, true, false, false},
    {blue_type, kElectric, true, false, false},
    
    // done
    {kNullType, kNullType, false, false, false}
  };

  // todo: add more, if I'm feeling bold

  // check stuff
  int failures = 0;
  const ChartCheckNode* test = &tests[0];
  while(test->typeToCheck != kNullType) {
    int result = doChartCheckNode(chart, *test);
    if(result != 0) {
      failures++;
    }
    test++;
  }


  return failures;
}
} // namespace test
} // namespace pokeman