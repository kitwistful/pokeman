/*
* resources.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/
#include "resources.hpp"

#include <cassert>
#include <cstring>

#include <iostream>

#include "pokeman_loader.hpp"

namespace pokeman {

struct TypeName {
  std::string name;
  Type type;
};

static const TypeName kTypenames[] = {
  { "Normal", kNormal },
  { "Fighting", kFighting },
  { "Flying", kFlying },
  { "Poison", kPoison },
  { "Ground", kGround },
  { "Rock", kRock },
  { "Bug", kBug },
  { "Ghost", kGhost },
  { "Steel", kSteel },
  { "Fire", kFire },
  { "Water", kWater },
  { "Grass", kGrass },
  { "Electric", kElectric },
  { "Psychic", kPsychic },
  { "Ice", kIce },
  { "Dragon", kDragon },
  { "Dark", kDark },
  { "Fairy", kFairy },
  { "NULL", kNullType }
};

struct MoveTypeName {
  std::string name;
  MoveType type;
};

static const MoveTypeName kMoveTypenames[] = {
  { "Physical", kPhysical },
  { "Special", kSpecial },
  { "Status", kStatus },
  { "NULL", kNullMoveType }
};

struct MoveTargetDescriptionName {
  std::string name;
  MoveTargetDescription code;
};

static const MoveTargetDescriptionName kMoveTargetDescriptionNames[] =  {
  {"Any Adjacent Foe", kAnyAdjacentFoe},
  {"All Adjacent Foes", kAllAdjacentFoes},
  {"All Foes", kAllFoes},
  {"Any Other", kAnyOther},
  {"All Adjacent", kAllAdjacent},
  {"All", kAll},
  {"Self", kSelf},
  {"Self Or Adjacent Ally", kSelfOrAdjacentAlly},
  {"Adjacent Ally", kAdjacentAlly},
  {"Whole Team", kWholeTeam},
  {"Any Adjacent", kAnyAdjacent},
  {"NULL", kNullTargetDescription}
};

struct TypeEffectivenessName {
  std::string name;
  TypeEffectiveness code;
};

static const TypeEffectivenessName kTimesNames[] = {
  {"0x", kZeroTimes},
  {"1x", kOneTimes},
  {"2x", kTwoTimes},
  {"4x", kFourTimes},
  {"1/2x", kHalfTimes},
  {"1/4x", kQuarterTimes},
  {"??x", kNullTimes}
};

static const TypeEffectivenessName kTimesDescriptionNames[] = {
  {"immune", kZeroTimes},
  {"regular", kOneTimes},
  {"super effective", kTwoTimes},
  {"doubly weak", kFourTimes},
  {"not very effective", kHalfTimes},
  {"doubly resistant", kQuarterTimes},
  {"NULLish", kNullTimes}
};


pokeman::resources::PokemanDatabase resources::initialize(bool* error_occured) {
  assert(error_occured != nullptr);
  Loader loader(FILEPATH_CONFIG);
  if(loader.errorOccured()) {
    *error_occured = true;
    return PokemanDatabase();
  } else {
    PokemanDatabase database;
    *error_occured = !database.load(loader);
    return database;
  }
}

std::string pokeman::resources::getTypeName(Type type) {
  const TypeName* name = &kTypenames[0];
  while(name->type != kNullType) {
    if(name->type == type) {
      return name->name;
    }
    name++;
  }
  return name->name;
}

Type resources::getTypeCode(const std::string & name) {
  const TypeName* current_name = &kTypenames[0];
  while(current_name->type != kNullType) {
    if(current_name->name.compare(name) == 0) {
      break;
    } else {
      current_name++;
    }
    // todo: ensure case sensitivity
  }
  return current_name->type;
}

std::string resources::getMoveTypeName(MoveType type) {
  const MoveTypeName* name = &kMoveTypenames[0];
  while(name->type != kNullMoveType) {
    if(name->type == type) {
      return name->name;
    }
    name++;
  }
  return name->name;
}

MoveType resources::getMoveTypeCode(const std::string & name) {
  const MoveTypeName* current_name = &kMoveTypenames[0];
  while(current_name->type != kNullMoveType) {
    if(current_name->name.compare(name) == 0) {
      break;
    } else {
      current_name++;
    }
    // todo: ensure case sensitivity
  }
  return current_name->type;
}

std::string resources::getMoveTargetName(MoveTargetDescription description) {
  const MoveTargetDescriptionName* name = &kMoveTargetDescriptionNames[0];
  while(name->code != kNullTargetDescription) {
    if(name->code == description) {
      return name->name;
    }
    name++;
  }
  return name->name;
}

MoveTargetDescription resources::getMoveTargetCode(const std::string & name) {
  const MoveTargetDescriptionName* current_name = &kMoveTargetDescriptionNames[0];
  while(current_name->code != kNullTargetDescription) {
    if(current_name->name.compare(name) == 0) {
      break;
    } else {
      current_name++;
    }
    // todo: ensure case sensitivity
  }
  return current_name->code;
}

std::string resources::getTypeEffectivenessTimes(TypeEffectiveness eff)  {
  const TypeEffectivenessName* name = &kTimesNames[0];
  while(name->code != kNullTimes) {
    if(name->code == eff) {
      return name->name;
    }
    name++;
  }
  return name->name;
}

std::string resources::getTypeEffectivenessDescription(TypeEffectiveness eff) {
  const TypeEffectivenessName* name = &kTimesDescriptionNames[0];
  while(name->code != kNullTimes) {
    if(name->code == eff) {
      return name->name;
    }
    name++;
  }
  return name->name;
}

TypeChart resources::generateTypeChartGen5() {
  // types
  TypeInfo normal, fighting, flying, poison, ground, rock, bug, ghost, steel,
    fire, water, grass, electric, psychic, ice, dragon, dark;
  
  // normal type
  normal.defending_[kFighting] = kTwoTimes;
  normal.defending_[kGhost] = kZeroTimes;

  // fighting type
  fighting.defending_[kFlying] = kTwoTimes;
  fighting.defending_[kRock] = kHalfTimes;
  fighting.defending_[kBug] = kHalfTimes;
  fighting.defending_[kPsychic] = kTwoTimes;
  fighting.defending_[kDark] = kHalfTimes;

  // flying type
  flying.defending_[kFighting] = kHalfTimes;
  flying.defending_[kGround] = kZeroTimes;
  flying.defending_[kRock] = kTwoTimes;
  flying.defending_[kBug] = kHalfTimes;
  flying.defending_[kGrass] = kHalfTimes;
  flying.defending_[kElectric] = kTwoTimes;
  flying.defending_[kIce] = kTwoTimes;

  // poison type
  poison.defending_[kFighting] = kHalfTimes;
  poison.defending_[kPoison] = kHalfTimes;
  poison.defending_[kGround] = kTwoTimes;
  poison.defending_[kBug] = kHalfTimes;
  poison.defending_[kGrass] = kHalfTimes;
  poison.defending_[kPsychic] = kTwoTimes;

  // ground type
  ground.defending_[kPoison] = kHalfTimes;
  ground.defending_[kRock] = kHalfTimes;
  ground.defending_[kWater] = kTwoTimes;
  ground.defending_[kGrass] = kTwoTimes;
  ground.defending_[kElectric] = kZeroTimes;
  ground.defending_[kIce] = kTwoTimes;

  // Rock type
  rock.defending_[kNormal] = kHalfTimes;
  rock.defending_[kFighting] = kTwoTimes;
  rock.defending_[kFlying] = kHalfTimes;
  rock.defending_[kPoison] = kHalfTimes;
  rock.defending_[kGround] = kTwoTimes;
  rock.defending_[kSteel] = kTwoTimes;
  rock.defending_[kFire] = kHalfTimes;
  rock.defending_[kWater] = kTwoTimes;
  rock.defending_[kGrass] = kTwoTimes;

  // Bug type
  bug.defending_[kFighting] = kHalfTimes;
  bug.defending_[kFlying] = kTwoTimes;
  bug.defending_[kGround] = kHalfTimes;
  bug.defending_[kRock] = kTwoTimes;
  bug.defending_[kFire] = kTwoTimes;
  bug.defending_[kGrass] = kHalfTimes;

  // Ghost type
  ghost.defending_[kNormal] = kZeroTimes;
  ghost.defending_[kFighting] = kZeroTimes;
  ghost.defending_[kPoison] = kHalfTimes;
  ghost.defending_[kBug] = kHalfTimes;
  ghost.defending_[kGhost] = kTwoTimes;
  ghost.defending_[kDark] = kTwoTimes;

  // Steel type
  steel.defending_[kNormal] = kHalfTimes;
  steel.defending_[kFighting] = kTwoTimes;
  steel.defending_[kFlying] = kHalfTimes;
  steel.defending_[kPoison] = kZeroTimes;
  steel.defending_[kGround] = kTwoTimes;
  steel.defending_[kRock] = kHalfTimes;
  steel.defending_[kBug] = kHalfTimes;
  steel.defending_[kGhost] = kHalfTimes;
  steel.defending_[kSteel] = kHalfTimes;
  steel.defending_[kFire] = kTwoTimes;
  steel.defending_[kGrass] = kHalfTimes;
  steel.defending_[kPsychic] = kHalfTimes;
  steel.defending_[kIce] = kHalfTimes;
  steel.defending_[kDragon] = kHalfTimes;
  steel.defending_[kDark] = kHalfTimes;

  // Fire type
  fire.defending_[kGround] = kTwoTimes;
  fire.defending_[kRock] = kTwoTimes;
  fire.defending_[kBug] = kHalfTimes;
  fire.defending_[kSteel] = kHalfTimes;
  fire.defending_[kFire] = kHalfTimes;
  fire.defending_[kWater] = kTwoTimes;
  fire.defending_[kGrass] = kHalfTimes;
  fire.defending_[kIce] = kHalfTimes;

  // Water Type
  water.defending_[kSteel] = kHalfTimes;
  water.defending_[kFire] = kHalfTimes;
  water.defending_[kWater] = kHalfTimes;
  water.defending_[kGrass] = kTwoTimes;
  water.defending_[kElectric] = kTwoTimes;
  water.defending_[kIce] = kHalfTimes;

  // Grass Type
  grass.defending_[kFlying] = kTwoTimes;
  grass.defending_[kPoison] = kTwoTimes;
  grass.defending_[kGround] = kHalfTimes;
  grass.defending_[kBug] = kTwoTimes;
  grass.defending_[kFire] = kTwoTimes;
  grass.defending_[kWater] = kHalfTimes;
  grass.defending_[kGrass] = kHalfTimes;
  grass.defending_[kElectric] = kHalfTimes;
  grass.defending_[kIce] = kTwoTimes;

  // Electric Type
  electric.defending_[kFlying] = kHalfTimes;
  electric.defending_[kGround] = kTwoTimes;
  electric.defending_[kSteel] = kHalfTimes;
  electric.defending_[kElectric] = kHalfTimes;

  // Psychic Type
  psychic.defending_[kFighting] = kHalfTimes;
  psychic.defending_[kBug] = kTwoTimes;
  psychic.defending_[kGhost] = kTwoTimes;
  psychic.defending_[kPsychic] = kHalfTimes;
  psychic.defending_[kDark] = kTwoTimes;

  // Ice Type
  ice.defending_[kFighting] = kTwoTimes;
  ice.defending_[kRock] = kTwoTimes;
  ice.defending_[kSteel] = kTwoTimes;
  ice.defending_[kFire] = kTwoTimes;
  ice.defending_[kIce] = kHalfTimes;

  // Dragon Type
  dragon.defending_[kFire] = kHalfTimes;
  dragon.defending_[kWater] = kHalfTimes;
  dragon.defending_[kGrass] = kHalfTimes;
  dragon.defending_[kElectric] = kHalfTimes;
  dragon.defending_[kIce] = kTwoTimes;
  dragon.defending_[kDragon] = kTwoTimes;

  // Dark Type
  dark.defending_[kFighting] = kTwoTimes;
  dark.defending_[kBug] = kTwoTimes;
  dark.defending_[kGhost] = kHalfTimes;
  dark.defending_[kPsychic] = kZeroTimes;
  dark.defending_[kDark] = kHalfTimes;

  // dummy Fairy type
  TypeInfo fairy;

  // Setup chart
  TypeChart chart;
  chart.setType(kNormal, normal);
  chart.setType(kFighting, fighting);
  chart.setType(kFlying, flying);
  chart.setType(kPoison, poison);
  chart.setType(kGround, ground);
  chart.setType(kRock, rock);
  chart.setType(kBug, bug);
  chart.setType(kGhost, ghost);
  chart.setType(kSteel, steel);
  chart.setType(kFire, fire);
  chart.setType(kWater, water);
  chart.setType(kGrass, grass);
  chart.setType(kElectric, electric);
  chart.setType(kPsychic, psychic);
  chart.setType(kIce, ice);
  chart.setType(kDragon, dragon);
  chart.setType(kDark, dark);
  chart.setType(kFairy, fairy);
  return chart;
}
std::map<int, MonsterSpecies> resources::generateGen5Species() {
  std::map<int, MonsterSpecies> species;

  // Leavanny
  MonsterSpecies leavanny;
  leavanny.name_ = "Leavanny";
  leavanny.type_ = TypesHad(kBug, kGrass);
  leavanny.valid_generations[4] = true;
  species[LEAVANNY_NUMBER] = leavanny;

  // Emboar
  MonsterSpecies emboar;
  emboar.name_ = "Emboar";
  emboar.type_ = TypesHad(kFire, kFighting);
  emboar.valid_generations[4] = true;
  species[EMBOAR_NUMBER] = emboar;

  // Krookodile
  MonsterSpecies krookodile;
  krookodile.name_ = "Krookodile";
  krookodile.type_ = TypesHad(kGround, kDark);
  krookodile.valid_generations[4] = true;
  species[KROOKODILE_NUMBER] = krookodile;

  // Floatzel
  MonsterSpecies floatzel;
  floatzel.name_ = "Floatzel";
  floatzel.type_ = TypesHad(kWater);
  floatzel.valid_generations[4] = true;
  species[FLOATZEL_NUMBER] = floatzel;

  // Crustle
  MonsterSpecies crustle;
  crustle.name_ = "Crustle";
  crustle.type_ = TypesHad(kBug, kRock);
  crustle.valid_generations[4] = true;
  species[CRUSTLE_NUMBER] = crustle;

  // Swanna
  MonsterSpecies swanna;
  swanna.name_ = "Swanna";
  swanna.type_ = TypesHad(kWater, kFlying);
  swanna.valid_generations[4] = true;
  species[SWANNA_NUMBER] = swanna;
  
  // todo: add all 6 pokemans
  // todo: If I ever use this for not Gen 5, update generations
  // to reflect unchanging
  // todo: when that becomes relevant, add movesets

  return species;
}
} //namespace resources

pokeman::resources::Loader::Loader() {
  // do nothing
}

pokeman::resources::Loader::Loader(const std::string & config_filepath) {
  loadConfig(config_filepath);
}

void pokeman::resources::Loader::loadConfig(const std::string & filepath) {
  YAML::Node node = loadFromFile(filepath);
  if(!errorOccured())
    parseConfig(node);
}

YAML::Node pokeman::resources::Loader::loadResource(LoaderTool::LoaderType type) {
  if(!typeHasFilepathSet(type)) {
    uninitialized_error_ = true;
    return YAML::Node();
  } else {
    return loadFromFile(filepath_.at(type));
  }
}

bool pokeman::resources::Loader::errorOccured() const {
  return bad_file_error_occured_ || uninitialized_error_ || unknown_error_occured_
    || config_parse_error_;
}

void pokeman::resources::Loader::parseConfig(YAML::Node node) {
  for(int i = 0; i < LoaderTool::kLoaderTypeSize; i++) {
    LoaderTool::LoaderType type = (LoaderTool::LoaderType)i;
    YAML::Node filename_node = node[LoaderTool::toString(type)];
    if(filename_node.IsDefined()) {
      try {
        filepath_[type] = filename_node.as<std::string>();
      } catch (YAML::BadConversion) {
        std::cerr << "[Error] extracting info from '" << LoaderTool::toString(type)
          << "' field" << std::endl;
        config_parse_error_ = true;
        return;
      }
    }
  }
  b_loaded_ = true;
}

YAML::Node pokeman::resources::Loader::loadFromFile(const std::string & filepath) {
  try {
    return YAML::LoadFile(filepath);
  } catch (YAML::ParserException& e) {
    std::cerr << "[Error] parsing " << filepath << std::endl 
      << "  line " << e.mark.line << " col " << e.mark.column << " pos " << e.mark.pos << std::endl
      << "  " << e.what() << std::endl;
    file_parser_error_ = true;
    return YAML::Node();
  } catch (YAML::BadFile& e) {
    std::cerr << "[Error] cannot load " << filepath << std::endl << "  "
      << e.what() << std::endl;
    bad_file_error_occured_ = true;
    return YAML::Node();
  } catch (YAML::Exception& e) {
    std::cerr << "[Error] loading " << filepath << std::endl << "  "
      << e.what() << std::endl;
    unknown_error_occured_ = true;
    return YAML::Node();
  }
}

bool pokeman::resources::Loader::typeHasFilepathSet(LoaderTool::LoaderType type) const {
  return filepath_.find(type) != filepath_.end();
}

std::string pokeman::resources::LoaderTool::toString(LoaderType type) {
  switch (type) {
  case kSpecies:
    return "species";
    break;

  case kTeam:
    return "team";
    break;

  case kMoves:
    return "moves";
    break;

  default:
    assert(false);
    return "???";
    break;
  }
}

pokeman::resources::LoaderTool::LoaderType pokeman::resources::LoaderTool::toCode(const std::string & string) {
  for(int i = 0; i < kLoaderTypeSize; i++) {
    std::string name = toString((LoaderType)i);
    if(name.compare(string) == 0) {
      return (LoaderType)i;
    }
  }
  return kLoaderTypeSize;
}

bool pokeman::resources::PokemanDatabase::load(Loader & loader) {
  loader_ = &loader;
  chart_ = generateTypeChartGen5(); // todo: file it?
  return loadSpecies() && loadMoves() && loadTeam();
}

const std::vector<pokeman::Monster>& pokeman::resources::PokemanDatabase::getTeam() const {
  return team_;
}

const std::map<std::string, pokeman::Move>& pokeman::resources::PokemanDatabase::getMoves() const {
  return moves_;
}

const pokeman::MonsterSpeciesLibrary & pokeman::resources::PokemanDatabase::getSpecies() const {
  return species_;
}

const pokeman::TypeChart & pokeman::resources::PokemanDatabase::getChart() const {
  return chart_;
}

bool pokeman::resources::PokemanDatabase::loadTeam() {
  MonsterParser parser;
  parser.setSpeciesLibrary(&species_);
  team_ = parser.parse(loader_->loadResource(resources::LoaderTool::kTeam));
  if(!parser.good()) {
    parser.getParserData().errorReport();
  }
  return !loader_->errorOccured() && parser.good();
}

bool pokeman::resources::PokemanDatabase::loadMoves() {
  MoveLibraryParser parser;
  moves_ = parser.parse(loader_->loadResource(resources::LoaderTool::kMoves));
  if(!parser.good()) {
    parser.getParserData().errorReport();
  }
  return !loader_->errorOccured() && parser.good();
}

bool pokeman::resources::PokemanDatabase::loadSpecies() {
  SpeciesParser parser;
  species_ = parser.parse(loader_->loadResource(resources::LoaderTool::kSpecies));
  if(!parser.good()) {
    parser.getParserData().errorReport();
  }
  return !loader_->errorOccured() && parser.good();
}
