/*
* resources.hpp
* Author: Kathryn McKay
* Poke Man Project
* May 2016
*
* Holds all the literal value content for the pokeman drivers.
*/

#ifndef POKEMAN_RESOURCES_HPP_
#define POKEMAN_RESOURCES_HPP_

#include <map>
#include <string>

#include <yaml-cpp/yaml.h>

#include "pokeman.hpp"


#define LEAVANNY_NUMBER 542
#define EMBOAR_NUMBER 500
#define KROOKODILE_NUMBER 553
#define FLOATZEL_NUMBER 419
#define CRUSTLE_NUMBER 558
#define SWANNA_NUMBER  581

#define FILEPATH_CONFIG "Data/config.yml"

namespace pokeman {
namespace resources {
namespace LoaderTool {
enum LoaderType {
  kSpecies,
  kTeam,
  kMoves,
  kLoaderTypeSize
};

std::string toString(LoaderType type);

LoaderType toCode(const std::string& string);
}


/// Contains info for loading
class Loader {
private:
  bool b_loaded_;
  std::map<LoaderTool::LoaderType, std::string> filepath_;

public:
  /// When true, file failed to load.
  bool bad_file_error_occured_;

  /// When true, file parsed bad.
  bool file_parser_error_;

  /// When true, initialization didn't happen.
  bool uninitialized_error_;

  /// When true, a config parse error happened.
  bool config_parse_error_;

  /// When true, unknown error occured.
  bool unknown_error_occured_;

public:
  /// Initializes as blank
  Loader();

  /// Loads configs
  Loader(const std::string& config_filepath);

  /// reads configuration.
  void loadConfig(const std::string& filepath);

  /// loads up node from file corresponding to the code
  YAML::Node loadResource(LoaderTool::LoaderType type);

  /// if true, last operation caused an error.
  bool errorOccured() const;

private:
  /// Loads in changes from config
  void parseConfig(YAML::Node node);

  /// Exception safe.
  YAML::Node loadFromFile(const std::string& filepath);

  /// false, if type does not exist in filepath map.
  bool typeHasFilepathSet(LoaderTool::LoaderType type) const;
};

class PokemanDatabase {
private:
  std::vector<Monster> team_;
  std::map<std::string, Move> moves_;
  MonsterSpeciesLibrary species_;
  resources::Loader* loader_;
  TypeChart chart_;

public:
  /// When true, successful.
  bool load(Loader& loader);

  /// retrieve reference to team
  const std::vector<Monster>& getTeam() const;

  /// retrieve reference to moves
  const std::map<std::string, Move>& getMoves() const;

  /// retrieve reference to species
  const MonsterSpeciesLibrary& getSpecies() const;

  const TypeChart& getChart() const;

private:
  bool loadTeam();

  bool loadMoves();

  bool loadSpecies();
};

/// Loads up all the resources from file.
PokemanDatabase initialize(bool* error_occured);

/// picks up string corresponding to a type
std::string getTypeName(Type type);

/// Converts string to type
Type getTypeCode(const std::string& name);

/// picks up movetype string corresponding to code
std::string getMoveTypeName(MoveType type);

/// Converts string to movetype
MoveType getMoveTypeCode(const std::string& name);

/// picks up move target string corresponding to code
std::string getMoveTargetName(MoveTargetDescription description);

/// Converts tring to move target
MoveTargetDescription getMoveTargetCode(const std::string& name);

/// Converts type effectiveness to 0x notation
std::string getTypeEffectivenessTimes(TypeEffectiveness eff);

/// Converts type effectiveness to an adjetive
std::string getTypeEffectivenessDescription(TypeEffectiveness eff);

/// Initializes a type chart.
TypeChart generateTypeChartGen5();

/// initalizes sample of Gen 5 pokemon by species
std::map<int, MonsterSpecies> generateGen5Species();

} //namespace pokeman
} // namespace resources

#endif //POKEMAN_RESOURCES_HPP_
