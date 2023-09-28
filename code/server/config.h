#ifndef CONFIG_H
#define CONFIG_H
#include <string>

class Config
{
private:
      /**
     * IP of the server.
     */
    std::string ip = "0.0.0.0";

    /**
     * Winner
    */
    std::string winner;
    
     /**
     * The name of team A.
     */
    std::string teamAName;

    /**
     * The path to the bot of team A.
     */
    std::string teamAFile;

    /**
     * Sockets of each team so that they can only act on their soldiers
    */
    SOCKET teamASocket;
    SOCKET teamBSocket;

    /**
     * The name of team B.
     */
    std::string teamBName;

    /**
     * The path to the bot of team B.
     */
    std::string teamBFile;

     /**
     * The map to run matches on.
     */
    std::string map;
public: 
    std::string getTeamAName() const { return teamAName; }
    void setTeamAName(const std::string &teamAName_) { teamAName = teamAName_; }
    
    std::string getTeamAFile() const { return teamAFile; }
    void setTeamAFile(const std::string &teamAFile_) { teamAFile = teamAFile_; }

    std::string getTeamBName() const { return teamBName; }
    void setTeamBName(const std::string &teamBName_) { teamBName = teamBName_; }

    std::string getTeamBFile() const { return teamBFile; }
    void setTeamBFile(const std::string &teamBFile_) { teamBFile = teamBFile_; }

    std::string getMap() const { return map; }
    void setMap(const std::string &map_) { map = map_; }

    std::string getIp() const { return ip; }
    void setIp(const std::string &ip_) { ip = ip_; }

    SOCKET getTeamASocket() const { return teamASocket; }
    void setTeamASocket(SOCKET teamAToken_) { teamASocket = teamAToken_; }

    SOCKET getTeamBSocket() const { return teamBSocket; }
    void setTeamBSocket(SOCKET teamBToken_) { teamBSocket = teamBToken_; }

    std::string getWinner() const { return winner; }
    void setWinner(const std::string &winner_) { winner = winner_; }
    
};

#endif // CONFIG_H
