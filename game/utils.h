#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <mutex>
#include <semaphore>

#include "soldier.h"
#include "dashboard.h"

class Soldier;
class Tile;
class Utils
{
private:
    static std::binary_semaphore semaphoreDB, semaphoreUsers;

    std::string db = __FILE__ "\\..\\..\\dataBases\\dataBase.db";
    std::string usersDb =  __FILE__ "\\..\\..\\dataBases\\users.db";

public:
    static std::vector<std::string> split(std::string s, std::string delimiter);
    std::vector<std::string> getFilesInDir(std::string dir);

    std::vector<std::string> getUsers();
    bool user_exists(std::string username);
    void create_user_table();
    bool verify_credentials(std::string username, std::string hashedPwd);
    bool create_user(std::string username, std::string hashedPwd);

    void saveSoldiersToSQL(std::vector<Soldier> soldiers);

    int insertSoldierToSQL(Soldier soldier);

    void updateSoldiersToSQL(std::vector<Soldier> soldiers);

    std::vector<Soldier> getSoldiersFromSql();

    std::vector<Soldier> getSoldiersAliveFromSql(std::string team);

    std::vector<Soldier> getBasesAliveFromSql();

    void saveTilesToSQL(std::vector<Tile> soldiers);

    void updateTilesToSQL(std::vector<Tile> tiles);

    std::vector<Tile> getTilesFromSql();
};

#endif // UTILS_H