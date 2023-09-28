#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <vector>
#include <fstream>
#include <iostream>
#include "location.h"
#include "soldier.h"
#include "tile.h"

class Soldier;
class Tile;
class Dashboard
{
private:
    std::vector<Tile> tiles;
    std::vector<Soldier> soldiers;

public:
    Dashboard(std::vector<Tile> map_) : tiles(map_) {};
    Dashboard();

    void addTile(Tile tile) {
        tiles.push_back(tile);
    }

    friend std::ostream& operator<<(std::ostream& os, const Dashboard& dashboard) {
        for (const auto& tile : dashboard.tiles) {
            os << tile.getPos().x << " " << tile.getPos().y << " " << tile.getType() << " " << tile.getOccupied() << std::endl;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Dashboard& dashboard) {
        int x, y;
        int type;
        bool occupied;
        while (is >> x >> y >> type >> occupied) {
            Tile tile(Location(x, y), type, occupied);
            dashboard.addTile(tile);
        }
        return is;
    }


    void static saveDashboard(const Dashboard& dashboard, const std::string& filename);
    bool static loadDashboard(Dashboard& dashboard, const std::string& filename);

    std::vector<Tile> getTiles() const { return tiles; }

    void setTiles(const std::vector<Tile> &tiles_) { tiles = tiles_; }

    static int const height = 20;
    static int const width = 20;

    std::vector<Soldier> getSoldiers() const { return soldiers; }
    void setSoldiers(const std::vector<Soldier> &soldiers_) { soldiers = soldiers_; }
};

#endif // DASHBOARD_H
