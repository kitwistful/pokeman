# Poke Man
Tools to calculate things for strategizing in RPG games, just for fun.

Whatever version number this project might find itself ascribed, I assure you
that there is quite a chance that it is not a release one.

The Data/ folder includes some test driver stuff.

# Features
- Type analysis: Get info on the elemental typing of each unit on the team,
    with notes on which matchups are the strongest and weakest.
- Moveset analysis: The computer will seek out a strong set of 4 moves for each
    party member, providing several recommendations.

# Dependencies
- yaml-cpp 0.5.2 (Can be downloaded from yaml.org)

# How to compile
There's nothing microsoft-specific or strange library features that I can
remember including with this source, by rights it could probably be loaded
into whatever legit IDE or compiler suits your fancy. For good measure, here's
some instructions on how to make a visual studio project out of it:
- Download the YAML source.
- Make a new Visual Studio Project (I'm using 2015. Don't worry, whatever
    you're using should be fine, right?).
- Add all the files in the pokeman src/ folder to the project, and also
    all the YAML ones.
- In the project Properties -> C/C++ -> Additional Include Directories, add
    the YAML include/ directory.
- Above C/C++, go to the Debugging settings and change the Working Directory to
    be the root pokeman directory, to ensure that the executable will be able
    to find the Data/ folder when running the program in debug mode from visual
    studio.
  - Take note of the 'Command Arguments' property, if you don't recognize it
      already. This can be used as parameters for the program.
  - Alternatively, you could just move the Data folder to the project directory
      or wherever you're running the executable from.
- Exit out of the properties, then press F5 to build and run.