#ifndef SOLDIER_H
#define SOLDIER_H

#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sqlite3.h>

#include "direction.h"
#include "location.h"
#include "dashboard.h"
#include "utils.h"

class Tile;
class Soldier
{
protected:
    /*
     * Type of soldier
    */
    std::string name;

    /**
     * Team the soldier is in
    */
    std::string team;

    /*
     * its index in the soldiers table
    */
    int index = -1;

    /*
     * Max health of the soldier
    */
    int maxHealth;

    /*
     * Health of the soldier
    */
    int healthPoints;

    /*
     * Attack of the soldier
    */
    int attackPoints;

    /*
     * Healing points of the soldier
    */
    int healingPoints;

    /*
     * Number of actions soldier can do per turn
    */
    int actions;

    /*
     * Number of actions soldier can do per turn
    */
    int actionsRemaining = 0;

    /*
    * Range at which soldier can attack
    */
    int range;
    /*
     * Location in the map of the soldier
    */
    Location pos;

    /*
     * Is able to be in a water tile
    */
    bool swims = false;

    /*
     * Is able to fly
    */
    bool flies = false;

    /*
     * Is able to create new soldiers
    */
    bool manufacturing = false;

    void assertCanMove(Direction dir);

    void assertCanHeal(Location pos);

    void assertCanAttack(Location pos);

    void assertCanCreate();
public:
    Soldier(int attack, int healingPoints_, int health, int actions_, int range_, Location l, bool swims_, bool flies_, bool manufacturing_ , std::string name_, std::string team_)
    : attackPoints(attack), healingPoints(healingPoints_), maxHealth(health), healthPoints(health), actions(actions_), range(range_), pos(l), swims(swims_), flies(flies_), manufacturing(manufacturing_) ,name(name_), team(team_) {}
    
    int getHealingPoints() const { return healingPoints; }
    
    int getActions() const { return actions; }

    int getHealthPoints() const { return healthPoints; }
    void setHealthPoints(int healthPoints_) { healthPoints = healthPoints_; }

    int getAttackPoints() const { return attackPoints; }

    int getMoveSpeed() const { return actions; }

    bool getSwims() const { return swims; }

    bool getFlies() const { return flies; }

    int getRange() const { return range; }

    bool getManufacturing() const { return manufacturing; }

    Location getPos() const { return pos; }
    void setPos(const Location &pos_) { pos = pos_; }

    std::string getName() const { return name; }

    void createSoldier(int id);
    void createTank();
    void createAttacker();
    void createHelicopter();
    void createHealer();

    void attack(Location pos);

    void heal(Location pos);

    void move(Direction dir);

    std::vector<Tile> getNearTiles();

    int getActionsRemaining() const { return actionsRemaining; }

    Location adjacentLocation(Direction dir);

    bool onTheMap(Location pos);

    bool isLocationOccupied(Location loc, std::vector<Tile>);
    
    int getIndex() const { return index; }
    void setIndex(int index_) { index = index_; }

    std::string getTeam() const { return team; }

    void setActionsRemaining(int actionsRemaining_) { actionsRemaining = actionsRemaining_; }

    int getMaxHealth() const { return maxHealth; }

    void setMaxHealth(int maxHealth_) { maxHealth = maxHealth_; }
};

class Helicopter : public Soldier
{
public:

    Helicopter(Location pos, std::string team_) : Soldier(6, 0, 15, 2, 3, pos, false, true, false, "Helicopter", team_) {}
    Helicopter(int x, int y, std::string team_) : Soldier(6, 0, 15, 2, 3, Location {x, y}, false, true, false, "Helicopter", team_) {}
};


class Healer : public Soldier
{
public:

    Healer(Location pos, std::string team_) : Soldier(0, 5, 10, 2, 2, pos, true, false, false, "Healer", team_) {}
    Healer(int x, int y, std::string team_) : Soldier(0, 5, 10, 2, 2, Location {x, y}, true, false, false, "Healer", team_) {}
};

class Base : public Soldier
{
public:

    Base(Location pos, std::string team_) : Soldier(0, 0, 500, 1, 1, pos, false, false, true, "Base", team_) {}
    Base(int x, int y, std::string team_) : Soldier(0, 0, 500, 1, 1, Location {x, y}, false, false, true, "Base", team_) {}
};

class Attacker : public Soldier
{
public:

    Attacker(Location pos, std::string team_) : Soldier(10, 0, 10, 3, 4, pos, false, false, false, "Attacker", team_) {}
    Attacker(int x, int y, std::string team_) : Soldier(10, 0, 10, 3, 4, Location {x, y}, false, false, false, "Attacker", team_) {}
};


class Tank : public Soldier
{
public:

    Tank(int x, int y, std::string team_) : Soldier(5, 0, 25, 1, 2, Location {x, y}, false, false, false, "Tank", team_) {}
    Tank(Location pos, std::string team_) : Soldier(5, 0, 25, 1, 2, pos, false, false, false, "Tank", team_) {}
};


#endif // SOLDIER_H
