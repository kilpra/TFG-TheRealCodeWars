#include "utils.h"

using namespace std;

binary_semaphore Utils::semaphoreDB{1};
binary_semaphore Utils::semaphoreUsers{1};

/**
 * <param name="s">             string you want to split.</param>
 * <param name="delimiter">     string with the delimiter.</param>
 * 
 * Split a string by the delimiter and return an array with the splitted string.
 * if string could not be splitten, return an array with s.
*/
vector<string> Utils::split(string s, string delimiter)
{
    size_t pos = 0;
    vector<string> out {};
    string token;

    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        out.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    out.push_back(s);

    return out;
}

bool Utils::user_exists(string username)
{
    semaphoreUsers.acquire();

    sqlite3* db;
    int rc;

    rc = sqlite3_open(this->usersDb.c_str(), &db);

    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
        semaphoreUsers.release();

        return false;
    }

    string query = "SELECT * FROM users WHERE username = ?";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        semaphoreUsers.release();

        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        semaphoreUsers.release();

        return false;
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        semaphoreUsers.release();

        return true;     
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreUsers.release();

    return false;
}

vector<string> Utils::getUsers()
{
    semaphoreUsers.acquire();
    vector<string> users;
    sqlite3* db;
    int rc;

    rc = sqlite3_open(this->usersDb.c_str(), &db);

    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
        semaphoreUsers.release();

        return users;
    }

    string query = "SELECT userName FROM users";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        semaphoreUsers.release();

        return users;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        users.push_back((char*)sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreUsers.release();

    return users;
}

bool Utils::verify_credentials(const string username, const string hashedPassword) {
    semaphoreUsers.acquire();

    sqlite3* db;
    int rc;

    rc = sqlite3_open(this->usersDb.c_str(), &db);

    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
        semaphoreUsers.release();

        return false;
    }

    string query = "SELECT hashed_password FROM users WHERE username = ?";

    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        semaphoreUsers.release();

        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (rc != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        semaphoreUsers.release();

        return false;
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW) {
        string storedHashedPassword(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));

        if (storedHashedPassword == hashedPassword) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            semaphoreUsers.release();

            return true;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreUsers.release();

    return false;
}

bool Utils::create_user(string username, string hashedPwd)
{
    semaphoreUsers.acquire();

    sqlite3* dataBase;
    char* errMsg = 0;

    int rc = sqlite3_open(this->usersDb.c_str(), &dataBase);

    if (rc != SQLITE_OK) {
        semaphoreUsers.release();

        return false;
    }

    std::string query = "INSERT INTO users (username, hashed_password) VALUES (\"" + username + "\", \"" + hashedPwd + "\")";

    rc = sqlite3_exec(dataBase, query.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        sqlite3_close(dataBase);
        semaphoreUsers.release();

        return false;
    }

    sqlite3_close(dataBase);
    semaphoreUsers.release();

    return true;
}

void Utils::create_user_table()
{
    semaphoreUsers.acquire();

    sqlite3* dataBase;
    char* errMsg = 0;

    int rc = sqlite3_open(this->usersDb.c_str(), &dataBase);

    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    string sql_statement (
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT,"
        "hashed_password TEXT);"
    );

    rc = sqlite3_exec(dataBase, sql_statement.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        sqlite3_close(dataBase);
        throw runtime_error("error while creating users");
    }

    sqlite3_close(dataBase);
    semaphoreUsers.release();
}


/**
 * Insert soldiers into table reseting first the table
*/
void Utils::saveSoldiersToSQL(std::vector<Soldier> soldiers)
{
    semaphoreDB.acquire();
    

    sqlite3* dataBase;
    char* errMsg = 0;

    int rc = sqlite3_open(this->db.c_str(), &dataBase);

    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    // drop the table
    rc = sqlite3_exec(dataBase, "DROP table soldiers", nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Error deleting rows from soldiers: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    string sql_statement (
        "CREATE TABLE IF NOT EXISTS soldiers ("
        "soldier_index INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "healthPoints INTEGER,"
        "maxHealth INTEGER,"
        "attackPoints INTEGER,"
        "healingPoints INTEGER,"
        "actions INTEGER,"
        "actionsRemaining INTEGER,"
        "range INTEGER,"
        "posX INTEGER,"
        "posY INTEGER,"
        "swims INTEGER,"
        "flies INTEGER,"
        "manufacturing INTEGER,"
        "team TEXT);"
    );

    rc = sqlite3_exec(dataBase, sql_statement.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        throw runtime_error(errMsg);
    }

    sql_statement = "INSERT INTO soldiers (name, healthPoints, attackPoints, healingPoints, actions, actionsRemaining, range, posX, posY, swims, flies, manufacturing, team, maxHealth) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(dataBase, sql_statement.c_str(), -1, &stmt, NULL);

    for (const auto soldier : soldiers) {
        sqlite3_bind_text(stmt, 1, soldier.getName().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, soldier.getHealthPoints());
        sqlite3_bind_int(stmt, 3, soldier.getAttackPoints());
        sqlite3_bind_int(stmt, 4, soldier.getHealingPoints());
        sqlite3_bind_int(stmt, 5, soldier.getActions());
        sqlite3_bind_int(stmt, 6, soldier.getActionsRemaining());
        sqlite3_bind_int(stmt, 7, soldier.getRange());
        sqlite3_bind_int(stmt, 8, soldier.getPos().x);
        sqlite3_bind_int(stmt, 9, soldier.getPos().y);
        sqlite3_bind_int(stmt, 10, soldier.getSwims() ? 1 : 0);
        sqlite3_bind_int(stmt, 11, soldier.getFlies() ? 1 : 0);
        sqlite3_bind_int(stmt, 12, soldier.getManufacturing() ? 1 : 0);
        sqlite3_bind_text(stmt, 13, soldier.getTeam().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 14, soldier.getMaxHealth());


        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            throw runtime_error("error while inserting soldier into table");
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(dataBase);
    
    semaphoreDB.release();
}

/**
 * Insert new soldier into table. No table reset
*/
int Utils::insertSoldierToSQL(Soldier soldier)
{
    semaphoreDB.acquire();

    sqlite3* dataBase;
    char* errMsg = 0;

    int rc = sqlite3_open(this->db.c_str(), &dataBase);

    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    string sql_statement (
        "CREATE TABLE IF NOT EXISTS soldiers ("
        "soldier_index INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT,"
        "healthPoints INTEGER,"
        "maxHealth INTEGER,"
        "attackPoints INTEGER,"
        "healingPoints INTEGER,"
        "actions INTEGER,"
        "actionsRemaining INTEGER,"
        "range INTEGER,"
        "posX INTEGER,"
        "posY INTEGER,"
        "swims INTEGER,"
        "flies INTEGER,"
        "manufacturing INTEGER,"
        "team TEXT);"
    );

    rc = sqlite3_exec(dataBase, sql_statement.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        throw runtime_error(errMsg);
    }

    sql_statement = "INSERT INTO soldiers (name, healthPoints, attackPoints, healingPoints, actions, actionsRemaining, range, posX, posY, swims, flies, manufacturing, team, maxHealth) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(dataBase, sql_statement.c_str(), -1, &stmt, NULL);

    sqlite3_bind_text(stmt, 1, soldier.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, soldier.getHealthPoints());
    sqlite3_bind_int(stmt, 3, soldier.getAttackPoints());
    sqlite3_bind_int(stmt, 4, soldier.getHealingPoints());
    sqlite3_bind_int(stmt, 5, soldier.getActions());
    sqlite3_bind_int(stmt, 6, soldier.getActionsRemaining());
    sqlite3_bind_int(stmt, 7, soldier.getRange());
    sqlite3_bind_int(stmt, 8, soldier.getPos().x);
    sqlite3_bind_int(stmt, 9, soldier.getPos().y);
    sqlite3_bind_int(stmt, 10, soldier.getSwims() ? 1 : 0);
    sqlite3_bind_int(stmt, 11, soldier.getFlies() ? 1 : 0);
    sqlite3_bind_int(stmt, 12, soldier.getManufacturing() ? 1 : 0);
    sqlite3_bind_text(stmt, 13, soldier.getTeam().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 14, soldier.getMaxHealth());

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        throw runtime_error("error while inserting soldier into table");
    }

    int index = sqlite3_last_insert_rowid(dataBase);

    sqlite3_finalize(stmt);
    sqlite3_close(dataBase);
    semaphoreDB.release();

    return index;
}

/**
 * Update soldiers table with the soldiers passed
*/
void Utils::updateSoldiersToSQL(std::vector<Soldier> soldiers)
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;

    int rc = sqlite3_open(this->db.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Error opening table" << std::endl;
        throw exception();
    }

    string sql_statement = "UPDATE soldiers SET healthPoints = ?, posX = ?, posY = ?, actionsRemaining = ? WHERE soldier_index = ?;";

    
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_statement.c_str(), -1, &stmt, NULL);

    for (const auto& soldier : soldiers) {
        sqlite3_bind_int(stmt, 1, soldier.getHealthPoints());
        sqlite3_bind_int(stmt, 2, soldier.getPos().x);
        sqlite3_bind_int(stmt, 3, soldier.getPos().y);
        sqlite3_bind_int(stmt, 4, soldier.getActionsRemaining());
        sqlite3_bind_int(stmt, 5, soldier.getIndex() + 1);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            throw runtime_error("error while inserting tile into table");
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();
}

/**
 * Get all soldiers form table
*/
std::vector<Soldier> Utils::getSoldiersFromSql()
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;
    vector<Soldier> soldiers;

    int rc = sqlite3_open(this->db.c_str(), &db);
    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    const char* sql_select = "SELECT attackPoints, healingPoints, healthPoints, actions, range, posX, posY, swims, flies, manufacturing, name, team, soldier_index, actionsRemaining, maxHealth FROM soldiers order by soldier_index asc";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Soldier soldier (
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4),
            Location(sqlite3_column_int(stmt, 5), sqlite3_column_int(stmt, 6)),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            (char*)sqlite3_column_text(stmt, 10),
            (char*)sqlite3_column_text(stmt, 11)
        );

        soldier.setIndex(sqlite3_column_int(stmt, 12) - 1);
        soldier.setActionsRemaining(sqlite3_column_int(stmt, 13));
        soldier.setMaxHealth(sqlite3_column_int(stmt, 14));
        soldiers.push_back(soldier);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();

    return soldiers;
}

/**
 * Get all soldiers alive form table and team <team>
*/
std::vector<Soldier> Utils::getSoldiersAliveFromSql(string team)
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;
    vector<Soldier> soldiers;

    int rc = sqlite3_open(this->db.c_str(), &db);
    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    string sql_select = "SELECT attackPoints, healingPoints, healthPoints, actions, range, posX, posY, swims, flies, manufacturing, name, team, soldier_index, actionsRemaining, maxHealth FROM soldiers WHERE team=\"" + team + "\" AND healthPoints > 0";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_select.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Soldier soldier (
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4),
            Location(sqlite3_column_int(stmt, 5), sqlite3_column_int(stmt, 6)),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            (char*)sqlite3_column_text(stmt, 10),
            (char*)sqlite3_column_text(stmt, 11)
        );

        soldier.setIndex(sqlite3_column_int(stmt, 12) - 1);
        soldier.setActionsRemaining(sqlite3_column_int(stmt, 13));
        soldier.setMaxHealth(sqlite3_column_int(stmt, 14));
        soldiers.push_back(soldier);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();

    return soldiers;
}

/**
 * Get bases alive from the table
*/
std::vector<Soldier> Utils::getBasesAliveFromSql()
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;
    vector<Soldier> soldiers;

    int rc = sqlite3_open(this->db.c_str(), &db);
    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    string sql_select = "SELECT attackPoints, healingPoints, healthPoints, actions, range, posX, posY, swims, flies, manufacturing, name, team, soldier_index, actionsRemaining, maxHealth FROM soldiers WHERE name=\"Base\" AND healthpoints > 0";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_select.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Soldier soldier (
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4),
            Location(sqlite3_column_int(stmt, 5), sqlite3_column_int(stmt, 6)),
            sqlite3_column_int(stmt, 7),
            sqlite3_column_int(stmt, 8),
            sqlite3_column_int(stmt, 9),
            (char*)sqlite3_column_text(stmt, 10),
            (char*)sqlite3_column_text(stmt, 11)
        );

        soldier.setIndex(sqlite3_column_int(stmt, 12) - 1);
        soldier.setActionsRemaining(sqlite3_column_int(stmt, 13));
        soldier.setMaxHealth(sqlite3_column_int(stmt, 14));
        soldiers.push_back(soldier);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();

    return soldiers;
}

/**
 * Insert tiles into table reseting first the table
*/
void Utils::saveTilesToSQL(std::vector<Tile> tiles)
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;

    int rc = sqlite3_open(this->db.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Error opening table" << std::endl;
        throw exception();
    }

    // Drop table
    rc = sqlite3_exec(db, "DROP table tiles", nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Error deleting rows from tiles: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    string sql_statement (
        "CREATE TABLE IF NOT EXISTS tiles ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "posX INTEGER,"
        "posY INTEGER,"
        "type INTEGER,"
        "occupied INTEGER,"
        "soldier_index INTEGER);"
    );

    rc = sqlite3_exec(db, sql_statement.c_str(), nullptr, 0, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "Error creating table tiles: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        throw exception();
    }

    sql_statement = "INSERT INTO tiles (posX, posY, type, occupied, soldier_index) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_statement.c_str(), -1, &stmt, NULL);

    for (const auto& tile : tiles) {
        sqlite3_bind_int(stmt, 1, tile.getPos().x);
        sqlite3_bind_int(stmt, 2, tile.getPos().y);
        sqlite3_bind_int(stmt, 3, tile.getType());
        sqlite3_bind_int(stmt, 4, tile.getOccupied() ? 1 : 0);
        sqlite3_bind_int(stmt, 5, tile.getSoldierIndex());
        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            throw runtime_error("error while inserting tile into table");
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();
}

std::vector<Tile> Utils::getTilesFromSql()
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;
    vector<Tile> tiles = vector<Tile> ();

    int rc = sqlite3_open(this->db.c_str(), &db);
    if (rc != SQLITE_OK) {
        throw runtime_error("error while opening table");
    }

    const char* sql_select = "SELECT * FROM tiles";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Tile tile(
            Location(sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2)),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4),
            sqlite3_column_int(stmt, 5)
        );

        tiles.push_back(tile);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();

    return tiles;
}

/**
 * Update tiles table with the tiles passed
*/
void Utils::updateTilesToSQL(std::vector<Tile> tiles)
{
    semaphoreDB.acquire();

    sqlite3* db;
    char* errMsg = 0;

    int rc = sqlite3_open(this->db.c_str(), &db);

    if (rc != SQLITE_OK) {
        std::cerr << "Error opening table" << std::endl;
        throw exception();
    }

    string sql_statement = "UPDATE tiles SET occupied = ?, soldier_index = ? WHERE posX = ? AND posY = ?;";

    
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(db, sql_statement.c_str(), -1, &stmt, NULL);

    for (const auto& tile : tiles) {
        sqlite3_bind_int(stmt, 1, tile.getOccupied() ? 1 : 0);
        sqlite3_bind_int(stmt, 2, tile.getSoldierIndex());
        sqlite3_bind_int(stmt, 3, tile.getPos().x);
        sqlite3_bind_int(stmt, 4, tile.getPos().y);

        rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE) {
            throw runtime_error("error while inserting tile into table");
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    semaphoreDB.release();
}