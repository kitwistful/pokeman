/*
* pokeman_loader.hpp
* Author: Kathryn McKay
* Pokeman Project
* May 2016
*
* Class that understands how to parse YAML into pokeman data storage.
*/
#ifndef POKEMAN_POKEMAN_LOADER_HPP_
#define POKEMAN_POKEMAN_LOADER_HPP_

#include <yaml-cpp/yaml.h>

#include "pokeman.hpp"

namespace pokeman {
namespace Parsing {
/// error codes for parsing
enum class Status {
  OK,
  Error,
  MissingRequiredFieldError,
  BadFieldValueError,
  NullStatus
};

/// extracts type data from scalar
Type parseType(YAML::Node scalar);

/// extracts type data from sequence of types
TypesHad parseTyping(YAML::Node sequence);

/// Extracts move type from node
MoveType parseMoveCategory(YAML::Node scalar);

/// Determines target from node
MoveTargetDescription parseTarget(YAML::Node scalar);
} // namespace Parsing

class ParserData {
public:
  /// When non-zero, indicates that an error has occured.
  Parsing::Status state_;

  /// indicates how many nodes have been read
  int node_count_;

  /// designates a name for the current node
  std::string current_node_description_;

  /// Constructor.
  ParserData();

  /// print out an error
  /// returns 1, for error
  int errorReport(YAML::Exception& exception) const;

  /// print out an error
  /// returns 1, for error
  int errorReport() const;
private:
  void printWhat() const;
  void printYAMLException(YAML::Exception& exception) const;
};

template<typename T>
class Parser {
protected:
  ParserData data_;
public:
  virtual ~Parser() {}

  ParserData getParserData() const { return data_; }

  bool good() const { return data_.state_ == Parsing::Status::OK; }

  void cleanState() { data_.state_ = Parsing::Status::OK; }

  virtual T parse(YAML::Node) = 0;

};

class TypeParser : public Parser<Type> {
public:
  Type parse(YAML::Node type_node) override;
};

class TypingParser : public Parser<TypesHad> {
private:
  TypeParser type_parser_;
public:
  TypesHad parse(YAML::Node type_node) override;
};

class MoveCategoryParser : public Parser<MoveType> {
public:
  MoveType parse(YAML::Node category_node) override;
};

class MoveTargetParser : public Parser<MoveTargetDescription> {
public:
  MoveTargetDescription parse(YAML::Node move_target_node) override;
};

class LearnsetParser : public Parser<std::vector<LearnsetMove>> {
private:
  std::map<std::string, LearnsetMove> learnset_map_;
public:
  std::vector<LearnsetMove> parse(YAML::Node learnset_root) override;

private:
  std::vector<LearnsetMove> learnsetMapToVector();

  void loadLearnsetOfLeveling(YAML::Node leveling_sequence);

  void loadLearnsetOfMachine(YAML::Node machine_sequence);

  void loadLearnsetOfTutoring(YAML::Node tutoring_sequence);

  bool moveExists(const std::string& name);

  void ensureMoveExists(const std::string& name);
};

class MonsterStatsParser : public Parser<MonsterStats> {
public:
  MonsterStats parse(YAML::Node map) override;
};

class SpeciesParser : public Parser<MonsterSpeciesLibrary> {
private:
  LearnsetParser learnset_parser_;
  MonsterStatsParser monster_stats_parser_;
public:
  /// Parse data in node into returned library.
  MonsterSpeciesLibrary parse(YAML::Node species_library_root) override;

private:
  /// Parse one species.
  MonsterSpecies parseSpecies(YAML::Node species_node);
};

class MonsterParser : public Parser<std::vector<Monster>> {
private:
  const MonsterSpeciesLibrary* species_library_;

public:
  MonsterParser(const MonsterSpeciesLibrary* species_library = nullptr);

  std::vector<Monster> parse(YAML::Node team_root) override;

  void setSpeciesLibrary(const MonsterSpeciesLibrary* species_library);

private:
  Monster parseMonster(YAML::Node monster_node);

  const MonsterSpecies* parseSpecies(YAML::Node species_node);

  std::vector<std::string> parseMoves(YAML::Node moves_sequence);

};

class MoveLibraryParser : public Parser<std::map<std::string, Move>> {
private:
  MoveTargetParser move_target_parser_;
  TypeParser type_parser_;
  MoveCategoryParser move_category_parser_;
  MonsterStatsParser monster_stats_parser_;

public:  
  std::map<std::string, Move> parse(YAML::Node moves_root) override;

private:
  Move parseMove(YAML::Node move_node);

  void parseStatus(YAML::Node status_node, Move* move);

};
} // namespace pokeman

#endif //POKEMAN_POKEMAN_LOADER_HPP