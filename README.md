# Through the Catacombs

Author: Eric Schneider

Design: This game was meant to test the limits of the PPU466. It uses both dynamic and static tile generation
to create a game that simulates 3D perspective while still only using the 2D spritemap.

Screen Shot:

![Screen Shot](screenshot.png)

How Your Asset Pipeline Works:

The game has two different kinds of source files, which both ship alongside it.
The first is a tilemap, which is simply loaded statically into tile memory in
row-major order, starting from the bottom left. Transparent pixels correspond to
palette index zero, black 1, grey 2, and white 3.

The map itself is also shipped as a PNG, and it is much simpler: black is a wall, and white is open space.

Indexing into the tilemap is done using a SpriteIndices class which holds the positions of all
the tiles by name. It's the same strategy Minecraft used to employ, and look how they turned out! :)

How To Play:

You are a guy running around a maze, trying to pick up as many emeralds as you can
in 60 seconds. Arrow keys to move. Your score is displayed in the top right,
and your time remaining is shown in the top left.

Sources: 

- Character is a modified version of a character from the [Kenney RPG Urban Pack](https://www.kenney.nl/assets/rpg-urban-pack)
- Further inspiration for sprites was taken from the [Kenney 1-Bit Pack](https://www.kenney.nl/assets/bit-pack)

This game was built with [NEST](NEST.md).

