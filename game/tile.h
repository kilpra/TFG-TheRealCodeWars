#ifndef TILE_H
#define TILE_H

#include <vector>
#include "location.h"


class Tile
{
private:
    Location pos;
    int type;
    bool occupied = false;
    int soldier_index = -1;

public:
    Tile(Location pos_) : pos(pos_){}
    Tile(Location pos_, int type_, bool occ) : pos(pos_), type(type_), occupied(occ) {}
    Tile(Location pos_, int type_, bool occ, int soldier_index_) : pos(pos_), type(type_), occupied(occ), soldier_index(soldier_index_) {}

    Tile(int x, int y) : pos(Location{x, y}){}
    Tile(int x, int y, int type_) : pos(Location{x, y}), type(type_) {}
    Tile(int x, int y, int type_, int soldier_index_) : pos(Location{x, y}), type(type_), soldier_index(soldier_index_){}

    static int const earth = 0;

    static int const water = 1;
    
    static int const lava = 2;

    Location getPos() const { return pos; }
    void setPos(const Location &pos_) { pos = pos_; }

    bool getOccupied() const { return occupied; }
    void setOccupied(bool occupied_) { occupied = occupied_; }

    int getType() const { return type; }

    int getSoldierIndex() const { return soldier_index; }
    void setSoldierIndex(int soldierIndex) { soldier_index = soldierIndex; }
};

#endif // TILE_H
