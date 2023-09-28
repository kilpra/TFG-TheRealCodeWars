#ifndef SERVER_H
#define SERVER_H
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <winsock2.h>
#include <chrono>
#include <unordered_map>
#include <filesystem>
#include <dirent.h>
#include <semaphore>
#include <chrono>

#include "httplib.h"

#include "config.h"
#include "../game/soldier.h"
#include "../game/utils.h"
#include "../game/dashboard.h"

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

class Server
{
private:
    bool game_running = false;
    Dashboard map;
    Config config;
public:
    Server(Dashboard map_) : map(map_) {};

    SOCKET socketTcp(int port);

    SOCKET socketListen(SOCKET sockfd);

    int socketTurn(SOCKET clientfd, std::vector<Soldier> soldiers, int timeout);

    std::string playTurns(int rounds, std::string team);
    void startServer(std::string ip);
    std::string runGame(int rounds, Dashboard dashboard);

    nlohmann::json soldierJson(Soldier soldier);
    nlohmann::json soldiersJson(std::vector<Soldier> soldiers);


    Dashboard getMap() const { return map; }

    void setMap(const Dashboard &map_) { map = map_; }

    bool gameRunning() const { return game_running; }
    void setGameRunning(bool gameRunning) { game_running = gameRunning; }
};

#endif // SERVER_H