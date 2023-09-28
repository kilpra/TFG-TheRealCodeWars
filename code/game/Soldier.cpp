#include "soldier.h"
using namespace std;

/**
 * Returns the adjacent Location in the provided direction
*/
Location Soldier::adjacentLocation(Direction dir)
{
    Location end(this->getPos());

    switch (dir.getDirection())
    {
        case Direction::up :
            end.y += 1;
            return end;

        case Direction::right :
            end.x += 1;
            return end;

        case Direction::down :
            end.y -= 1;
            return end;

        case Direction::left :
            end.x -= 1;
            return end;
    }

    return end;
}

/**
 * Check if Location is on the map
*/
bool Soldier::onTheMap(Location loc)
{
    if (loc.x >= Dashboard::width || loc.x < 0 || loc.y >= Dashboard::height || loc.y < 0)
    {
        return false;
    }
    
    return true;
}

bool Soldier::isLocationOccupied(Location loc, vector<Tile> tiles)
{    
    return tiles.at(loc.x * Dashboard::width + loc.y).getOccupied();
}

/**
 * Check if pos is a valid location and throws exception if not
*/
void Soldier::assertCanMove(Direction dir)
{
    if (this->actionsRemaining <= 0) {
        throw out_of_range("Soldier can not do more actions");
    }

    if (this->name == "Base") {
        throw out_of_range("Base can not move");
    }

    Location loc = adjacentLocation(dir);

    if (!onTheMap(loc)) {
        throw out_of_range("Can only move to locations on the map");
    }

    Utils utils;
    auto tiles = utils.getTilesFromSql();

    if (isLocationOccupied(loc, tiles)) {
        throw out_of_range("Cannot move to an occupied location");
    }

    switch (tiles.at(loc.x * Dashboard::width + loc.y).getType())
    {
        case Tile::water:
            if (! (this->swims || this->flies)) {
                throw out_of_range("Cannot move to water tile");
            }
            
            break;
        case Tile::lava:
            if (! this->flies) {
                throw out_of_range("Cannot move to lava tile");
            }

        default:
            break;
    }
    
}

/*
 * Moves soldier in the direction passed.
 * returns location to which the soldier moved 
*/
void Soldier::move(Direction dir)
{
    assertCanMove(dir);

    Utils utils;
    auto soldiers = utils.getSoldiersFromSql();
    auto tiles = utils.getTilesFromSql();

    --this->actionsRemaining;
    soldiers.at(this->index).setActionsRemaining(this->actionsRemaining);

    // Save old pos
    int xOld = this->pos.x;
    int yOld = this->pos.y;

    this->pos = this->adjacentLocation(dir);
    
    // Update soldier pos
    soldiers.at(this->index).setPos(this->pos);

    vector<Soldier> changedS;
    changedS.push_back(soldiers.at(this->index));
    utils.updateSoldiersToSQL(changedS);

    vector<Tile> changed;

    // Occupy tile
    tiles.at(this->pos.x * Dashboard::width + this->pos.y).setOccupied(true);
    tiles.at(this->pos.x * Dashboard::width + this->pos.y).setSoldierIndex(this->index);
    changed.push_back(tiles.at(this->pos.x * Dashboard::width + this->pos.y));

    // Unoccupy tile
    tiles.at(xOld * Dashboard::width + yOld).setOccupied(false);
    tiles.at(xOld * Dashboard::width + yOld).setSoldierIndex(-1);
    changed.push_back(tiles.at(xOld * Dashboard::width + yOld));
    
    utils.updateTilesToSQL(changed);

    return;
}

/*
 * Throws exception if soldier can not heal pos
*/
void Soldier::assertCanHeal(Location loc)
{
    if (this->actionsRemaining <= 0) {
        throw out_of_range("Soldier can not do more actions");
    }

    if (this->healingPoints == 0)
    {
        throw out_of_range("Soldier with no healing points can not heal");
    }

    if (!onTheMap(loc)) {
        throw out_of_range("Can only heal locations on the map");
    }
    
    int distance = abs(this->pos.x - loc.x) + abs(this->pos.y - loc.y);

    if (distance > range)
    {
        throw out_of_range("Location healed is out of Soldier range");
    }
}

/**
 * Heals a location, if location is not occupied or is occupied by a enemy unit or by a Base, soldier will lose that action.
*/
void Soldier::heal(Location loc)
{
    assertCanHeal(loc);

    Utils utils;
    auto soldiers = utils.getSoldiersFromSql();
    auto tiles = utils.getTilesFromSql();
    
    --this->actionsRemaining;
    soldiers.at(this->index).setActionsRemaining(this->actionsRemaining);

    vector<Soldier> changedS;
    changedS.push_back(soldiers.at(this->index));

    int index = tiles.at(loc.x * Dashboard::width + loc.y).getSoldierIndex();

    if (index >= 0 && soldiers.at(index).getTeam() == this->team && "Base" != soldiers.at(index).getName())
    {
        int hp = soldiers.at(index).getHealthPoints() + this->healingPoints;

        if (hp > soldiers.at(index).getMaxHealth())
        {
            soldiers.at(index).setHealthPoints(soldiers.at(index).getMaxHealth());
        } else {
            soldiers.at(index).setHealthPoints(hp);
        }

        changedS.push_back(soldiers.at(index));
    }

    utils.updateSoldiersToSQL(changedS);
}

/*
 * Throws exception if soldier can not attack pos
*/
void Soldier::assertCanAttack(Location loc)
{
    if (this->actionsRemaining <= 0) {
        throw out_of_range("Soldier can not do more actions");
    }


    if (!onTheMap(loc)) {
        throw out_of_range("Can only attack locations on the map");
    }
    
    int distance = abs(this->pos.x - loc.x) + abs(this->pos.y - loc.y);

    if (distance > range)
    {
        throw out_of_range("Location attacked is out of Soldier range");
    }
}

/**
 * Attacks a location, if location is not occupied or is occupied by a friendly unit, soldier will lose that action.
*/
void Soldier::attack(Location loc)
{
    assertCanAttack(loc);

    Utils utils;
    auto soldiers = utils.getSoldiersFromSql();
    auto tiles = utils.getTilesFromSql();
    
    --this->actionsRemaining;
    soldiers.at(this->index).setActionsRemaining(this->actionsRemaining);

    vector<Soldier> changedS;
    changedS.push_back(soldiers.at(this->index));

    auto tile = tiles.at(loc.x * Dashboard::width + loc.y);
    int enemyIndex = tile.getSoldierIndex();

    if (enemyIndex >= 0 && (soldiers.at(enemyIndex).getTeam() != this->team))
    {
        int hp = soldiers.at(enemyIndex).getHealthPoints() - this->attackPoints;
        soldiers.at(enemyIndex).setHealthPoints(hp);

        changedS.push_back(soldiers.at(enemyIndex));
        
        if (hp < 1)
        {
            tile.setOccupied(false);
            tile.setSoldierIndex(-1);

            vector<Tile> changedT;
            changedT.push_back(tile);
            utils.updateTilesToSQL(changedT);
        }
    }

    utils.updateSoldiersToSQL(changedS);
}

/*
 * Throws exception if unit can not create soldiers
*/
void Soldier::assertCanCreate()
{
    if (this->actionsRemaining <= 0) {
        throw out_of_range("Unit can not do more actions");
    }

    if (! this->manufacturing) {
        throw out_of_range("Unit can not create soldiers");
    }

    Location loc(-1, -1);
    Utils utils;
    auto tiles = utils.getTilesFromSql(); 

    for (int i = 1; i < 5; i++)
    {
        loc = adjacentLocation(Direction (i));
        if(onTheMap(loc) && !isLocationOccupied(loc, tiles)) {
            break;
        }
    }
    
    if (!onTheMap(loc) || isLocationOccupied(loc, tiles))
    {
        throw out_of_range("There are not any empty tiles close by to create a unit");
    }
}

/**
 * Creates a soldier of the provided type 
*/
void Soldier::createSoldier (int id)
{
    this->assertCanCreate();

    Utils utils;
    auto soldiers = utils.getSoldiersFromSql();
    auto tiles = utils.getTilesFromSql();
    int index;

    --this->actionsRemaining;
    soldiers.at(this->index).setActionsRemaining(this->actionsRemaining);
    vector<Soldier> changedS;
    changedS.push_back(soldiers.at(this->index));
    utils.updateSoldiersToSQL(changedS);

    Location loc(-1, -1);

    for (int i = 1; i < 5; i++)
    {
        loc = adjacentLocation(i);
        if(onTheMap(loc) && !isLocationOccupied(loc, tiles)) {
            break;
        }
    }
    
    if (!onTheMap(loc) || isLocationOccupied(loc, tiles))
    {
        return;
    }
    
    switch (id)
    {
    case 0:
        {
            Tank soldier(loc.x, loc.y, this->getTeam());

            index = utils.insertSoldierToSQL(soldier);
    
            soldier.setIndex(index - 1);

            soldiers.push_back(soldier);

            // Occupy tile
            tiles.at(loc.x * Dashboard::width + loc.y).setOccupied(true);

            tiles.at(loc.x * Dashboard::width + loc.y).setSoldierIndex(soldier.getIndex());

            vector<Tile> changed;
            changed.push_back(tiles.at(loc.x * Dashboard::width + loc.y));
            utils.updateTilesToSQL(changed);

            break;
        }
    case 1:
        {
            Attacker soldier(loc.x, loc.y, this->getTeam());

            index = utils.insertSoldierToSQL(soldier);
    
            soldier.setIndex(index - 1);

            soldiers.push_back(soldier);

            // Occupy tile
            tiles.at(loc.x * Dashboard::width + loc.y).setOccupied(true);

            tiles.at(loc.x * Dashboard::width + loc.y).setSoldierIndex(soldier.getIndex());

            vector<Tile> changed;
            changed.push_back(tiles.at(loc.x * Dashboard::width + loc.y));
            utils.updateTilesToSQL(changed);

            break;
        }
    case 2:
        {
            Helicopter soldier(loc.x, loc.y, this->getTeam());

            index = utils.insertSoldierToSQL(soldier);
    
            soldier.setIndex(index - 1);

            soldiers.push_back(soldier);

            // Occupy tile
            tiles.at(loc.x * Dashboard::width + loc.y).setOccupied(true);

            tiles.at(loc.x * Dashboard::width + loc.y).setSoldierIndex(soldier.getIndex());

            vector<Tile> changed;
            changed.push_back(tiles.at(loc.x * Dashboard::width + loc.y));
            utils.updateTilesToSQL(changed);

            break;
        }
    case 3:
        { 
            Healer soldier(loc.x, loc.y, this->getTeam());
            
            index = utils.insertSoldierToSQL(soldier);
    
            soldier.setIndex(index - 1);

            soldiers.push_back(soldier);

            // Occupy tile
            tiles.at(loc.x * Dashboard::width + loc.y).setOccupied(true);

            tiles.at(loc.x * Dashboard::width + loc.y).setSoldierIndex(soldier.getIndex());

            vector<Tile> changed;
            changed.push_back(tiles.at(loc.x * Dashboard::width + loc.y));
            utils.updateTilesToSQL(changed);

            break;
        }
    }
}

void Soldier::createTank(){createSoldier(0);}
void Soldier::createAttacker(){createSoldier(1);}
void Soldier::createHelicopter(){createSoldier(2);}
void Soldier::createHealer(){createSoldier(3);}

/**
 * Returns the tiles close to the soldier that are in his range
*/
vector<Tile> Soldier::getNearTiles()
{
    Utils utils;
    vector<Tile> tiles = utils.getTilesFromSql();
    vector<Tile> out;

    for (int i = -this->range; i <= this->range; i++)
    {
        for (int j = -this->range; j <= this->range; j++)
        {
            if (abs(i) + abs(j) <= this->range && onTheMap(Location(this->pos.x + i, this->pos.y + j)))
            {
                out.push_back(tiles.at((this->pos.x + i) * Dashboard::width + this->pos.y + j));
            }
        }
    }
    
    return out;
}