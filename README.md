Introduction
============

Monster RPG 2 is a JRPG by Nooskewl. The git history is incomplete. Sorry.

See http://nooskewl.ca/monster-rpg-2 for more info.



Building
========

The build process is not streamlined much yet. You need to know what you're doing.

Prerequisites
-------------

- Allegro 5 (git version)
- Lua 5.2
- BASS is used by default but Allegro can be used for audio instead (see CMakeLists.txt)

Platforms
---------

The game should work on Windows, Mac OS X, Linux, Android and iOS. 

Basic Process
-------------

- To build for Windows, you'll need the prerequisites first. Then use CMake to generate a project (MSVC and MinGW should work but we use MSVC for official builds.)
- data.zip needs to be created by zipping the data/ directory (you will have a data/ toplevel directory in the ZIP file)
- You'll need to put the datafile with the EXE and BASS DLL (if applicable) to run the game