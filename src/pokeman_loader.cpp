/*
* pokeman_loader.cpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*/
#include "pokeman_loader.hpp"

#include <cassert>

#include <iostream>

#include "resources.hpp"

using namespace std::literals::string_literals;

namespace pokeman {
namespace Parsing {
Type parseType(YAML::Node scalar) {
  TypeParser parser;
  Type typing = parser.parse(scalar);
  if(!parser.good()) {
    return kNullType;
  } else {
    return typing;
  }
}

TypesHad parseTyping(YAML::Node sequence) {
  TypingParser parser;
  TypesHad typing = parser.parse(sequence);
  if(!parser.good()) {
    return kNullType;
  } else {
    return typing;
  }
}

MoveType parseMoveCategory(YAML::Node scalar) {
  MoveCategoryParser parser;
  MoveType category = parser.parse(scalar);
  if(!parser.good()) {
    return kNullMoveType;
  } else {
    return category;
  }
}

MoveTargetDescription parseTarget(YAML::Node scalar) {
  MoveTargetParser parser;
  MoveTargetDescription typing = parser.parse(scalar);
  if(!parser.good()) {
    return kNullTargetDescription;
  } else {
    return typing;
  }
}

} // namespace Parsing



//         private functions


template <typename T>
T valueOrDefault(YAML::Node scalar, const T& defval) {
  try {
    return scalar.as<T>();
  } catch (YAML::BadConversion) {
    return defval;
  }
}

template <typename T>
T valueOrError(YAML::Node scalar, ParserData* data) {
  assert(data != nullptr);
  try {
    return scalar.as<T>();
  } catch (YAML::BadConversion& e) {
    data->state_ = Parsing::Status::BadFieldValueError;
    data->current_node_description_ += " "s + e.what();
    return T();
  }
}

template <typename T>
T parseOrDefault(Parser<T>* parser, YAML::Node node, const T& defval) {
  assert(parser != nullptr);
  T value = parser->parse(node);
  bool go_default = !parser->good();
  if(go_default) {
    parser->cleanState();
    return defval;
  } else {
    return value;
  }
}

template <typename T>
T parseOrError(Parser<T>* parser, YAML::Node node, ParserData* data) {
  assert(parser != nullptr);
  assert(data != nullptr);
  T value = parser->parse(node);
  if(!parser->good()) {
    data->state_ = Parsing::Status::BadFieldValueError;
    return T();
  } else {
    return value;
  }
}

//        finish private functions

int ParserData::errorReport(YAML::Exception& exception) const {
  printWhat();
  printYAMLException(exception);
  return 1;
}

int ParserData::errorReport() const {
  printWhat();
  return 1;
}

void ParserData::printWhat() const {
  std::cerr << "[Error] Parsing: " << current_node_description_ << std::endl
    << " nodes scanned " << node_count_ << " status code: " << (int)state_ << std::endl;
}

void ParserData::printYAMLException(YAML::Exception & exception) const {
  if(!exception.mark.is_null()) {
    std::cerr << "  Ln " << exception.mark.line << " Col "
      << exception.mark.column << " Pos"
      << exception.mark.pos << std::endl;
  }
  std::cerr << exception.what() << std::endl;
}

ParserData::ParserData() {
  state_ = Parsing::Status::OK;
  node_count_ = 0;
  current_node_description_ = "...";
}

Type TypeParser::parse(YAML::Node type_node) {
  data_.current_node_description_ = "Type";
  std::string type_name = valueOrError<std::string>(type_node, &data_);
  if(!good()) {
    return kNullType;
  } else {
    Type type = resources::getTypeCode(type_name);
    if(type == kNullType) {
      data_.current_node_description_ = type_name;
      data_.state_ = Parsing::Status::BadFieldValueError;
    }
    return type;
  }
}

MoveType MoveCategoryParser::parse(YAML::Node category_node) {
  data_.current_node_description_ = "Move Category";
  std::string type_name = valueOrError<std::string>(category_node, &data_);
  if(!good()) {
    return kNullMoveType;
  } else {
    MoveType category = resources::getMoveTypeCode(type_name);
    if(category == kNullMoveType) {
      data_.current_node_description_ = type_name;
      data_.state_ = Parsing::Status::BadFieldValueError;
    }
    return category;
  }
}

MoveTargetDescription MoveTargetParser::parse(YAML::Node move_target_node) {
  data_.current_node_description_ = "Move Target";
  std::string type_name = valueOrError<std::string>(move_target_node, &data_);
  if(!good()) {
    return kNullTargetDescription;
  } else {
    MoveTargetDescription target = resources::getMoveTargetCode(type_name);
    if(target == kNullTargetDescription) {
      data_.current_node_description_ = type_name;
      data_.state_ = Parsing::Status::BadFieldValueError;
    }
    return target;
  }
}

MonsterSpeciesLibrary SpeciesParser::parse(YAML::Node species_library_root) {
  // initialize values
  data_.current_node_description_ = "species library";
  data_.node_count_ = 0;
  data_.current_node_description_ = "...";
  
  // fill species
  MonsterSpeciesLibrary species;
  int species_count = (int)species_library_root.size();
  for(int i = 0; i < species_count; i++) {
    YAML::Node species_node = species_library_root[i];

    // read number
    int species_number = valueOrError<int>(species_node["number"], &data_);
    if(!good()) {
      data_.current_node_description_ = "reading number";
      return MonsterSpeciesLibrary();
    }

    // read species
    MonsterSpecies species_data = parseSpecies(species_node);
    if(!good())
      return MonsterSpeciesLibrary();

    // iterate
    species.set(species_number, species_data);
    data_.node_count_++;
  }

  // done
  return species;
}

MonsterSpecies SpeciesParser::parseSpecies(YAML::Node species_node) {
  MonsterSpecies species;

  // get name
  species.name_ = valueOrError<std::string>(species_node["name"], &data_);
  if(!good()) {
    return MonsterSpecies();
  }

  // get types
  species.type_ = Parsing::parseTyping(species_node["types"]);

  // read in moveset
  species.learnset_ = learnset_parser_.parse(species_node["learnset"]);

  // get base stats
  species.base_stats_ = monster_stats_parser_.parse(species_node["basestats"]);

  // todo: add generations, if I ever start reading pokemon from certain generations.
  // done
  return species;
}

std::vector<LearnsetMove> LearnsetParser::parse(YAML::Node learnset_root) {std::map<std::string, LearnsetMove> learnset;
  data_.current_node_description_ = "Learnset";
  learnset_map_.clear();
  loadLearnsetOfLeveling(learnset_root["leveling"]);
  loadLearnsetOfMachine(learnset_root["machine"]);
  loadLearnsetOfTutoring(learnset_root["tutoring"]);
  return learnsetMapToVector();
}

std::vector<LearnsetMove> LearnsetParser::learnsetMapToVector() {
  std::vector<LearnsetMove> learnset_list;
  for(std::pair<std::string, LearnsetMove> pair : learnset_map_) {
    learnset_list.push_back(pair.second);
  }
  return learnset_list;
}

void LearnsetParser::loadLearnsetOfLeveling(YAML::Node leveling_sequence) {
  int move_count = (int)leveling_sequence.size();
  for(int i = 0; i < move_count; i++) {
    YAML::Node leveling_scalar = leveling_sequence[i];

    // load move name
    std::string move_name = valueOrError<std::string>(leveling_scalar["name"], &data_);
    if(!good()) {
      return;
    }

    // load move level
    int level = valueOrError<int>(leveling_scalar["level"], &data_);
    if(!good()) {
      data_.current_node_description_ = move_name;
      return;
    }
    
    // do it
    ensureMoveExists(move_name);
    learnset_map_.at(move_name).learned_at_level_ = level;
  }
}

void LearnsetParser::loadLearnsetOfMachine(YAML::Node machine_sequence) {
  int move_count = (int)machine_sequence.size();
  for(int i = 0; i < move_count; i++) {
    YAML::Node machine_scalar = machine_sequence[i];

    // load move name
    std::string move_name = valueOrError<std::string>(machine_scalar, &data_);
    if(!good()) {
      return;
    }
    // do it
    ensureMoveExists(move_name);
    learnset_map_.at(move_name).b_machine_able_ = true;
  }
}

void LearnsetParser::loadLearnsetOfTutoring(YAML::Node tutoring_sequence) {
  int move_count = (int)tutoring_sequence.size();
  for(int i = 0; i < move_count; i++) {
    YAML::Node tutoring_scalar = tutoring_sequence[i];

    // load move name
    std::string move_name = valueOrError<std::string>(tutoring_scalar, &data_);
    if(!good()) {
      return;
    }
    
    // do it
    ensureMoveExists(move_name);
    learnset_map_.at(move_name).b_tutor_able_ = true;
  }
}

bool LearnsetParser::moveExists(const std::string & name) {
  return learnset_map_.find(name) != learnset_map_.end();
}

void LearnsetParser::ensureMoveExists(const std::string & name) {
  if(!moveExists(name)) {
    learnset_map_[name] = LearnsetMove(name);
  }
}

MonsterParser::MonsterParser(const MonsterSpeciesLibrary * species_library) : species_library_(species_library) {}

std::vector<Monster> MonsterParser::parse(YAML::Node team_root) {
  assert(species_library_ != nullptr);
  data_.current_node_description_ = "Monster Team";
  std::vector<Monster> team;
  const int team_count = (int)team_root.size();
  for(int i = 0; i < team_count; i++) {
    Monster monster = parseMonster(team_root[i]);
    if(!good()) {
      return std::vector<Monster>();
    } else {
      team.push_back(monster);
    }
  }

  // done
  return team; 
}

void MonsterParser::setSpeciesLibrary(const MonsterSpeciesLibrary * species_library) {
  species_library_ = species_library;
}

Monster MonsterParser::parseMonster(YAML::Node monster_node) {
  // Load species
  data_.current_node_description_ = "Monster";
  const MonsterSpecies* species = parseSpecies(monster_node["species"]);
  if(!good() || species == nullptr) {
    data_.state_ = Parsing::Status::BadFieldValueError;
    return Monster();
  } else {
    data_.current_node_description_ = species->name_;
  }

  // Load moves
  std::vector<std::string> moves = parseMoves(monster_node["moves"]);
  if(!good()) {
    return Monster();
  }

  // get name
  std::string name = valueOrDefault(monster_node["name"], species->name_);

  // make monster
  Monster monster(name, species);
  monster.hm_moves_ = moves;
  return monster;
}

const MonsterSpecies * MonsterParser::parseSpecies(YAML::Node species_node) { 
  // get species name
  std::string species_name = valueOrError<std::string>(species_node, &data_);
  if(!good()) {
    return nullptr;
  }

  // get species pointer
  const MonsterSpecies* species_ptr = species_library_->get(species_name);
  if(species_ptr == nullptr) {
    data_.current_node_description_ = "Pointer for "s + data_.current_node_description_ + " species "s + species_name;
    data_.state_ = Parsing::Status::BadFieldValueError;
    return nullptr;
  } else {
    return species_ptr;
  }

}

std::vector<std::string> MonsterParser::parseMoves(YAML::Node moves_sequence) {
  std::vector<std::string> moves;
  int n_moves = (int)moves_sequence.size();
  for(int i = 0; i < n_moves; i++) {
    std::string move = valueOrError<std::string>(moves_sequence[i], &data_);
    if(!good()) {
      return std::vector<std::string>();
    } else {
      moves.push_back(move);
    }
  }
  return moves;
}

std::map<std::string, Move> MoveLibraryParser::parse(YAML::Node moves_root) {
  std::map<std::string, Move> moves;
  int move_count = (int)moves_root.size();
  for(int i = 0; i < move_count; i++) {
    // get name
    std::string move_name = valueOrError<std::string>(moves_root[i]["name"], &data_);
    if(!good()) {
      return std::map<std::string, Move>();
    } else {
      data_.current_node_description_ = move_name;
    }
    
    // get move
    Move move = parseMove(moves_root[i]);
    if(!good()) {
      return std::map<std::string, Move>();
    } else {
      moves[move_name] = move;
    }
  }
  return moves;
}

Move MoveLibraryParser::parseMove(YAML::Node move_node) {

  // Setup move with optionals
  Move move;
  move.target_ = parseOrDefault(&move_target_parser_, move_node["target"], kAnyAdjacent);
  move.power_ = valueOrDefault<int>(move_node["power"], -1);
  move.accuracy_ = valueOrDefault<int>(move_node["accuracy"], -1);

  // parse move name
  move.name_ = valueOrError<std::string>(move_node["name"], &data_);
  if(!good()) {
    return Move();
  } else {
    data_.current_node_description_ = move.name_;
  }

  // parse move PP
  move.pp_ = valueOrError<int>(move_node["pp"], &data_);
  if(!good()) {
    return Move();
  }

  // parse move type
  move.elemental_type_ = parseOrError(&type_parser_, move_node["type"], &data_);
  if(!good()) {
    return Move();
  }

  // parse move category
  move.move_type_ = parseOrError(&move_category_parser_, move_node["category"], &data_);
  if(!good()) {
    return Move();
  }

  // Retrieve stat modifier node
  parseStatus(move_node["status"], &move);

  // done
  return move;
}

void MoveLibraryParser::parseStatus(YAML::Node status_node, Move * move) {
  assert(move != nullptr);
  if(!status_node.IsDefined()) {
    return;
  } else {
    move->status_effect_chance_self_ = move->status_effect_change_target_ = 100; // todo: take a look at this, if status effects become important.
    
    move->stat_modifiers_target_ = monster_stats_parser_.parse(status_node["target"]);
    if(MoveTarget(move->target_).target_self) // propogate self-target, just in case.
      move->stat_modifiers_self_ = move->stat_modifiers_target_;
    else // load own stat changes.
      move->stat_modifiers_self_ = monster_stats_parser_.parse(status_node["self"]);
  }
}

TypesHad TypingParser::parse(YAML::Node type_node) {
  int number_of_types = type_node.size();
  data_.current_node_description_ = "Typing";
  if (number_of_types < 0 || number_of_types > 2) {
    std::cerr << "Error: invalid number of types: " << number_of_types 
      << std::endl;
    data_.state_ = Parsing::Status::Error;
    data_.current_node_description_ = "there were like "s + std::to_string(number_of_types) + " shitty things"s;
    return TypesHad(kNullType);
  } else {
    // first type
    Type type = type_parser_.parse(type_node[0]);
    if(!type_parser_.good()) {
      data_.state_ = Parsing::Status::BadFieldValueError;
      return kNullType;
    }

    // dual or mono type?
    if(number_of_types == 1) {
      return type;
    } else {
      // second type
      Type type2 = type_parser_.parse(type_node[1]);
      if(!type_parser_.good()) {
        data_.state_ = Parsing::Status::BadFieldValueError;
        return kNullType;
      }
      
      return TypesHad(type, type2);
    }
  }
}

MonsterStats MonsterStatsParser::parse(YAML::Node map) {
  MonsterStats stats;
  stats.hp_ = valueOrDefault(map["hp"], 0);
  stats.attack_ = valueOrDefault(map["attack"], 0);
  stats.defense_ = valueOrDefault(map["defense"], 0);
  stats.special_attack_ = valueOrDefault(map["spatk"], 0);
  stats.special_defense_ = valueOrDefault(map["spdef"], 0);
  stats.speed_ = valueOrDefault(map["speed"], 0);
  return stats;
}

}// namespace pokeman