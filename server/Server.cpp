#include "server.h"
using namespace std;
using json = nlohmann::json;

// Semaphore used for turns between teams
binary_semaphore semaphoreA{0}, semaphoreB{0};

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

std::string generate_session_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    std::stringstream ss;
    ss << std::hex;

    // Generate 32 random hex chars
    for (int i = 0; i < 32; ++i) {
        int random_value = dis(gen);
        ss << random_value;
    }

    return ss.str();
}

void Server::startServer(string ip)
{
    httplib::Server svr;
    string dir = __FILE__;
    unordered_map<string, string> sessions;

    svr.Get("/", [dir](const httplib::Request &, httplib::Response &res) {
        auto dir1 = dir + "\\..\\html\\index.html";
        ifstream file (dir1);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/html");
        }
    });

    svr.Post("/checkUser", [&sessions](const httplib::Request &req, httplib::Response &res) {
        string username = req.get_param_value("user");
        Utils utils;
       
        if (utils.user_exists(username)) {
            res.set_content("false", "text/plain");
        } else {
            res.set_content("true", "text/plain");
        }
    });

    svr.Get("/getUsers", [&sessions](const httplib::Request &req, httplib::Response &res) {
        Utils utils;
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                vector<string> users = utils.getUsers();
                json j;

                for(string user : users) {
                    j += user;
                }

                res.set_content(j.dump(), "application/json");
                return;
            }
        }

        res.status = 403;
        res.set_content("You don't have acces to this page", "text/plain");
    });

    svr.Get("/register", [dir](const httplib::Request &, httplib::Response &res) {
        auto dir1 = dir + "\\..\\html\\register.html";
        ifstream file (dir1);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/html");
        }
    });

    svr.Get("/createGame",[dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                auto dir1 = dir + "\\..\\html\\createGame.html";
                ifstream file (dir1);

                if (!file.is_open())
                {
                    res.set_content("Internal Error", "text/plain");
                } else {
                    stringstream htmlBuf;
                    htmlBuf << file.rdbuf();
                    file.close();
                    res.set_content(htmlBuf.str(), "text/html");
                }
                return;
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Post("/register", [&sessions](const httplib::Request &req, httplib::Response &res) {
        string username = req.get_param_value("username");
        string password = req.get_param_value("password");
        Utils utils;
       
        if (utils.create_user(username, password)) {
            res.set_redirect("/login");
        } else {
            res.set_redirect("/register");
        }
    });

    svr.Get("/home",[dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                auto dir1 = dir + "\\..\\html\\home.html";
                ifstream file (dir1);

                if (!file.is_open())
                {
                    res.set_content("Internal Error", "text/plain");
                } else {
                    stringstream htmlBuf;
                    htmlBuf << file.rdbuf();
                    file.close();
                    res.set_content(htmlBuf.str(), "text/html");
                }
                return;
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Get("/tutorial",[dir](const httplib::Request &req, httplib::Response &res){
        
        auto dir1 = dir + "\\..\\html\\tutorial.html";
        ifstream file (dir1);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/html");
        }
    });

    svr.Get("/RealCodeWars.py", [dir](const httplib::Request &req, httplib::Response &res) {
        auto dir1 = dir + "\\..\\..\\bots\\runningGame\\RealCodeWars.py";

        ifstream file (dir1, ios::binary);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/plain");
        }
    });

    svr.Get("/ExampleBot.py", [dir](const httplib::Request &req, httplib::Response &res) {
        auto dir1 = dir + "\\..\\..\\bots\\users\\Example\\ExampleBot.py";

        ifstream file (dir1, ios::binary);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/plain");
        }
    });

    svr.Post("/deleteFile",[this, dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                string dir2 = dir + "\\..\\..\\bots\\users\\" + user;

                if (req.get_file_value("File").content != "") {
                    dir2 = dir2 + "\\" + req.get_file_value("File").content;

                    if (remove(dir2.c_str()) != 0) {
                        res.set_content("Error removing file " + dir2, "text/plain");
                        res.status = 404;
                        return;
                    }
                    res.set_content("File removed: " + dir2, "text/plain");
                    return;
                }
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Get("/getMapsNames",[dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                string dir2 = dir + "\\..\\..\\maps";

                DIR* directory = opendir(dir2.c_str());

                if (directory) {
                    dirent* file;
                    json j;
                    // Iterate through directory and add all regular file names to json
                    while ((file = readdir(directory)) != nullptr) {
                        string filePath = dir2 + string("/") + file->d_name;
                        struct stat fileStat;
                        if (stat(filePath.c_str(), &fileStat) == 0) {
                            if (S_ISREG(fileStat.st_mode)) {  // Regular file
                                j += file->d_name;
                            }
                        }
                    }

                    closedir(directory);
                    res.set_content(j.dump(), "application/json");
                    return;
                }

                res.set_content("Internal Error: Failed to open the directiory: " + dir2, "text/plain");
                return;
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });


    svr.Post("/getFilesNames",[dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                if (req.get_file_value("user").content != "") {
                    user = req.get_file_value("user").content;
                }

                string dir2 = dir + "\\..\\..\\bots\\users\\" + user;

                DIR* directory = opendir(dir2.c_str());

                if (directory) {
                    dirent* file;
                    json j;
                    // Iterate through directory and add all regular file names to json
                    while ((file = readdir(directory)) != nullptr) {
                        string filePath = dir2 + string("/") + file->d_name;
                        struct stat fileStat;
                        if (stat(filePath.c_str(), &fileStat) == 0) {
                            if (S_ISREG(fileStat.st_mode)) {  // Regular file
                                j += file->d_name;
                            }
                        }
                    }

                    closedir(directory);
                    res.set_content(j.dump(), "application/json");
                    return;
                }

                res.set_content("Internal Error: Failed to open the directiory: " + dir2, "text/plain");
                return;
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Post("/getFile", [dir, &sessions](const httplib::Request &req, httplib::Response &res) {
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                if (req.get_file_value("FileName").content != ""){
                    string dir2 = dir + "\\..\\..\\bots\\users\\" + user + "\\" + req.get_file_value("FileName").content;
                    ifstream file (dir2);

                    if (!file.is_open())
                    {
                        res.set_content("Internal Error", "text/plain");
                    } else {
                        stringstream htmlBuf;
                        htmlBuf << file.rdbuf();
                        file.close();
                        res.set_content(htmlBuf.str(), "text/plain");
                    }
                    return;
                }
            }
        }

        res.set_redirect("/login");
    });

    svr.Get("/code",[dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                auto dir1 = dir + "\\..\\html\\code.html";
                ifstream file (dir1);

                if (!file.is_open())
                {
                    res.set_content("Internal Error", "text/plain");
                } else {
                    stringstream htmlBuf;
                    htmlBuf << file.rdbuf();
                    file.close();
                    res.set_content(htmlBuf.str(), "text/html");
                }
                return;
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Post("/code",[this, dir, &sessions](const httplib::Request &req, httplib::Response &res){
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                string dir2 = dir + "\\..\\..\\bots\\users\\" + user;

                // Try to create Dir, it will fail if it already exists and work just fine.
                auto created = filesystem::create_directory(dir2);

                string filename, content;
                ofstream outfile;

                // Save .py files
                if (req.get_file_value("File").content != "" && req.get_file_value("FileName").content != "")
                {
                    filename = "\\" + req.get_file_value("FileName").content;
                    content = req.get_file_value("File").content;
                    
                    outfile.open(dir2 + filename, std::ios::out | std::ios::binary);

                    if (! outfile.is_open())
                    {
                        res.set_content("Internal Error", "text/plain");
                        return;
                    }

                    outfile.write(content.c_str(), content.size());
                    outfile.close();
                }
                
                return;
            }
        }
        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Get("/login", [dir, &sessions](const httplib::Request &req, httplib::Response &res) {
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                res.set_redirect("/home");
                return;
            }
        }

        auto dir1 = dir + "\\..\\html\\login.html";
        ifstream file (dir1);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/html");
        }
    });

    svr.Post("/login",[dir, &sessions](const httplib::Request &req, httplib::Response &res) {
        string username = req.get_param_value("username");
        string password = req.get_param_value("password");
        Utils utils;
       
        if (utils.verify_credentials(username, password)) {
            // Generate a UID
            string session_id = generate_session_id();

            sessions[username] = session_id;

            res.set_header("Set-Cookie", "session_id=" + session_id);
            res.set_header("Set-Cookie", "user=" + username);

            res.set_redirect("/home");
        } else {
            res.set_redirect("/login");
        }
    });

    svr.Get("/winner", [dir](const httplib::Request &, httplib::Response &res) {
        auto dir1 = dir + "\\..\\html\\winner.html";
        ifstream file (dir1);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/html");
        }
    });

    // Provides the image with the name writen after 'images/' (e.g images/logo.jpg)
    svr.Get(R"(/images/(.+))", [dir](const httplib::Request &req, httplib::Response &res) {
        string image = req.matches[1];
        auto dir1 = dir + "\\..\\html\\images\\" + image;

        ifstream file (dir1, ios::binary);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            string format = image.substr(image.find('.'));

            if (format == ".svg")
            {
                format = "image/svg+xml";
            } else if (format == ".png")
            {
                format = "image/png";
            } else {
                format = "image/jpeg";
            }  
            file.close();
            res.set_content(htmlBuf.str(), format);
        }
    });

    // Provides the css with the name writen after 'css/' (e.g css/default.css)
    svr.Get(R"(/css/(.+))", [dir](const httplib::Request &req, httplib::Response &res) {
        string image = req.matches[1];
        auto dir1 = dir + "\\..\\html\\css\\" + image;

        ifstream file (dir1, ios::binary);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/css");
        }
    });

    // Provides the .js with the name writen after 'javaScript/' (e.g javaScript/game.js)
    svr.Get(R"(/javaScript/(.+))", [dir](const httplib::Request &req, httplib::Response &res) {
        string js = req.matches[1];
        auto dir1 = dir + "\\..\\html\\JavaScript\\" + js;

        ifstream file (dir1, ios::binary);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "application/javascript");
        }
    });

    svr.Get("/watchGame", [dir](const httplib::Request &req, httplib::Response &res) {
        auto dir1 = dir + "\\..\\html\\game.html";
        ifstream file (dir1);

        if (!file.is_open())
        {
            res.set_content("Internal Error", "text/plain");
        } else {
            stringstream htmlBuf;
            htmlBuf << file.rdbuf();
            file.close();
            res.set_content(htmlBuf.str(), "text/html");
        }
    });

    svr.Post("/runGame", [dir, this, &sessions](const httplib::Request &req, httplib::Response &res) {
        string session_id = req.get_header_value("Cookie");

        vector<string> cookies = Utils::split(session_id, ";");

        if (cookies.size() == 2) {
            string user;

            if (cookies[1].find("user") != string::npos)
            {
                user = cookies[1].substr(cookies[1].find("=") + 1);
                session_id = cookies[0].substr(cookies[0].find("=") + 1);
            } else {
                session_id = cookies[1].substr(cookies.at(1).find("=") + 1);
                user = cookies[0].substr(cookies.at(0).find("=") + 1);
            }

            if (sessions.find(user) != sessions.end() && sessions[user] == session_id) {
                string dir2 = dir + "\\..\\..\\bots\\users\\";
                string filename, content;
                ofstream outfile;

                if (this->gameRunning()) {
                    res.set_redirect("/createGame");
                    return;
                }

                if (req.get_file_value("codeA").content == "" || req.get_file_value("codeB").content == "" ||
                    req.get_file_value("userA").content == "" || req.get_file_value("userB").content == ""
                ) {
                    res.set_content("code not found", "text/plain");
                    res.status = 404;
                    return;
                }

                this->config.setTeamAFile(dir2 + req.get_file_value("userA").content + "\\" + req.get_file_value("codeA").content);

                this->config.setTeamBFile(dir2 + req.get_file_value("userB").content + "\\" + req.get_file_value("codeB").content);
                
                this->config.setTeamAName(req.get_file_value("userA").content);

                this->config.setTeamBName(req.get_file_value("userB").content);

                this->config.setMap(req.get_file_value("map").content);

                int rounds = 100;

                if (req.get_file_value("rounds").content != "")
                {
                    try{
                    rounds = stoi(req.get_file_value("rounds").content);
                    } catch(const exception& e) {
                        res.set_content("could not convert " + req.get_file_value("rounds").content + " to integer", "text/plain");
                        return;
                    }
                }

                dir2 = dir + "\\..\\..\\maps\\" + this->config.getMap();

                vector<Tile> tiles;

                Dashboard dashboard(tiles);

                if (! Dashboard::loadDashboard(dashboard, dir2)) {
                    res.set_content("could not open file " + dir2, "text/plain");
                    res.status = 503;
                    return;
                }

                string winner = runGame(rounds, dashboard);

                res.set_content("Game done. Winner is: " + winner, "text/plain");
            }
        }

        res.status = 403;
        res.set_redirect("/login");
    });

    svr.Get("/gameStatus", [this](const httplib::Request &req, httplib::Response &res) {
        json j;

        if (! this->config.getWinner().empty()) {
            j["winner"] = this->config.getWinner();
            res.set_content(j.dump(), "application/json");
        }
        
        auto soldiers = this->getMap().getSoldiers();
        for(Tile tile : this->getMap().getTiles()) {
            int x = tile.getPos().x;
            int y = tile.getPos().y;

            j[y][x]["x"] = x;
            j[y][x]["y"] = y;
            j[y][x]["type"] = tile.getType();

            // Check if soldier is set to avoid segmentation fault error
            if(tile.getOccupied() && (tile.getSoldierIndex() >= 0)) {
                j[y][x]["soldier"]["name"] = soldiers.at(tile.getSoldierIndex()).getName();
                j[y][x]["soldier"]["attack"] = soldiers.at(tile.getSoldierIndex()).getAttackPoints();
                j[y][x]["soldier"]["healing"] = soldiers.at(tile.getSoldierIndex()).getHealingPoints();
                j[y][x]["soldier"]["health"] = soldiers.at(tile.getSoldierIndex()).getHealthPoints();
                j[y][x]["soldier"]["maxHealth"] = soldiers.at(tile.getSoldierIndex()).getMaxHealth();
                j[y][x]["soldier"]["team"] = soldiers.at(tile.getSoldierIndex()).getTeam();
            }
        }

        res.set_content(j.dump(), "application/json");
    });

    svr.Get("/teamNames", [this](const httplib::Request &req, httplib::Response &res) {
        json j;
        auto file = Utils::split(this->config.getTeamAFile(), "\\");
        j["TeamA"] = this->config.getTeamAName() + "(" + file[file.size() - 1] + ")";
        file = Utils::split(this->config.getTeamBFile(), "\\");
        j["TeamB"] = this->config.getTeamBName() + "(" + file[file.size() - 1] + ")";

        res.set_content(j.dump(), "application/json");
    });

    svr.listen(ip, 80);
}

string Server::runGame(int rounds, Dashboard dashboard)
{
    if (this->gameRunning())
    {
        return "Error";
    }
    
    Utils utils;
    string winner;
    this->config.setWinner(winner);
    // Initialize DataBase
    utils.saveTilesToSQL(dashboard.getTiles());
    utils.saveSoldiersToSQL(dashboard.getSoldiers());
    this->setGameRunning(true);

    this->map = dashboard;

    string botsDir = (string) __FILE__ + "\\..\\..\\bots\\runningGame\\";
    // Remove previous files
    auto file = botsDir + "botA.py";
    remove(file.c_str());
    file = botsDir + "botB.py";
    remove(file.c_str());
    
    auto aFile = this->config.getTeamAFile();

    filesystem::copy(this->config.getTeamAFile(), botsDir + "botA.py");
    filesystem::copy(this->config.getTeamBFile(), botsDir + "botB.py");

    SOCKET socketfd = socketTcp(8080);

    // Execute .py codes:
    thread botA([](string command){system(command.c_str());}, "python " + botsDir + "botA.py");
    this->config.setTeamASocket(socketListen(socketfd));

    json enemyBase = {{"posX", 18}, {"posY", 18}};
    string message = enemyBase.dump();
    char buffer[256];
    int n;

    send(config.getTeamASocket(), message.c_str(), (int)strlen(message.c_str()), 0);
    n = recv(config.getTeamASocket(), buffer, sizeof(buffer), 0);
    if (n < 0) error("ERROR reading socket confA");

    thread botB([](string command){system(command.c_str());}, "python " + botsDir + "botB.py");
    this->config.setTeamBSocket(socketListen(socketfd));

    enemyBase = {{"posX", 1}, {"posY", 1}};
    message = enemyBase.dump();

    send(config.getTeamBSocket(), message.c_str(), (int)strlen(message.c_str()), 0);
    n = recv(config.getTeamBSocket(), buffer, sizeof(buffer), 0);
    if (n < 0) error("ERROR reading socket confB");

    // Set Bases
    auto tiles = this->map.getTiles();
    auto soldiers = this->map.getSoldiers();

    int x, y;
    string team = "";
    vector<Tile> changedTiles;

    x = 1;
    y = 1;
    team = "TeamA";

    Base a (x, y, team);

    int index = utils.insertSoldierToSQL(a);

    if(index == 0) {
        cerr << "Invalid Index 0 returned from insert Soldier, finishing game" << endl;

        return "Error";
    }

    a.setIndex(index - 1);          
    soldiers.push_back(a);

    // Occupy tile
    tiles.at(x * Dashboard::width + y).setOccupied(true);
    tiles.at(x * Dashboard::width + y).setSoldierIndex(a.getIndex());
    changedTiles.push_back(tiles.at(x * Dashboard::width + y));

    x = 18;
    y = 18;
    team = "TeamB";

    Base b(x, y, team);

    index = utils.insertSoldierToSQL(b);

    if(index == 0) {
        cerr << "Invalid Index 0 returned from insert Soldier, finishing game" << endl;

        return "Error";
    }

    b.setIndex(index - 1);          

    soldiers.push_back(b);

    // Occupy tile
    tiles.at(x * Dashboard::width + y).setOccupied(true);
    tiles.at(x * Dashboard::width + y).setSoldierIndex(b.getIndex());
    changedTiles.push_back(tiles.at(x * Dashboard::width + y));

    utils.updateTilesToSQL(changedTiles);

    this->map.setSoldiers(soldiers);
    this->map.setTiles(tiles);

    // reset semaphores
    semaphoreB.release();
    semaphoreA.release();
    semaphoreA.acquire();

    // Play turns
    thread teamA([this, &winner](int rounds, string team){winner = playTurns(rounds, team);}, rounds, "TeamA");
    thread teamB([this, &winner](int rounds, string team){winner = playTurns(rounds, team);}, rounds, "TeamB");

    teamA.join();
    teamB.join();

    send(this->config.getTeamASocket(), "EXIT\n", 5, 0);
    send(this->config.getTeamBSocket(), "EXIT\n", 5, 0);

    botA.join();
    botB.join();

    closesocket(this->config.getTeamASocket());
    closesocket(this->config.getTeamBSocket());
    closesocket(socketfd);
    WSACleanup();

    // resest map
    vector<Tile> map;
    Dashboard d(map);
    this->setMap(d);

    this->config.setWinner(this->config.getTeamBName() + "(" + winner + ")");
    if (winner == "TeamA") {
        this->config.setWinner(this->config.getTeamAName() + "(" + winner + ")");
    } 
    
    cout << "Game Over. Winner is: " << this->config.getWinner() << endl;
    this->setGameRunning(false);
    return this->config.getWinner();
}

string Server::playTurns(int rounds, string team) 
{
    Utils utils;
    for (int i = 0; i < rounds; i++)
    {
        if (team == "TeamA") {
            // Try to acquire for 2s
            if(!semaphoreB.try_acquire_for(chrono::seconds(2))) {
                cout << "2 seconds elapsed waiting for turn, other team took too long, playing turn" << endl;
            }
        } else {
             // Try to acquire for 2s
            if(!semaphoreA.try_acquire_for(chrono::seconds(2))) {
                cout << "2 seconds elapsed waiting for turn, other team took too long, playing turn" << endl;
            }
        }


        auto bases = utils.getBasesAliveFromSql();

        if (bases.size() == 1)
        {
            return bases[0].getTeam();
        }
        

        auto soldiers = utils.getSoldiersAliveFromSql(team);
        vector<Soldier> updateS;

        for(int i = 0; i< soldiers.size(); i++) {
            // reset soldier actions
            soldiers[i].setActionsRemaining(soldiers[i].getActions());

            updateS.push_back(soldiers[i]);
        }

        utils.updateSoldiersToSQL(updateS);

        if (team == "TeamA")
        {
            socketTurn(this->config.getTeamASocket(), soldiers, soldiers.size() * 250);
        } else {
            socketTurn(this->config.getTeamBSocket(), soldiers, soldiers.size() * 250);
        }

        // update map to update web interface
        this->map.setTiles(utils.getTilesFromSql());
        this->map.setSoldiers(utils.getSoldiersFromSql());


        if (team == "TeamA") {
            semaphoreA.release();
        } else {
            semaphoreB.release();
        }
    }
    auto bases = utils.getBasesAliveFromSql();
    
    auto winner = bases[0];

    for(auto base : bases) {
        if (base.getHealthPoints() > winner.getHealthPoints())
        {
            winner = base;
        }
    }

    return winner.getTeam();
}

nlohmann::json Server::soldierJson(Soldier soldier)
{
    nlohmann::json json_body = {
        {"name", soldier.getName()},
        {"swims", soldier.getSwims()},
        {"flies", soldier.getFlies()},
        {"range", soldier.getRange()},
        {"attack", soldier.getAttackPoints()},
        {"healingPoints", soldier.getHealingPoints()},
        {"health", soldier.getHealthPoints()},
        {"maxHealth", soldier.getMaxHealth()},
        {"team", soldier.getTeam()},
        {"index", soldier.getIndex()},
        {"posX", soldier.getPos().x},
        {"posY", soldier.getPos().y},
        {"actionsRemaining", soldier.getActionsRemaining()}
    };

    return json_body;
}

nlohmann::json Server::soldiersJson(vector<Soldier> soldiers)
{
    json json_body;
    for (auto soldier : soldiers)
    {
        json_body += soldierJson(soldier);
    }

    return json_body;
}


SOCKET Server::socketTcp(int port)
{
    WSADATA wsa;
    struct sockaddr_in serv_addr;
    SOCKET sockfd;

    // Inicializamos la biblioteca Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Error while starting Winsock\n");
        return 1;
    }

    // creamos el socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET)
    {
    error("ERROR opening socket");
    }

    // limpiamos
    memset(&serv_addr, 0, sizeof(serv_addr));

    // Conexion a Internet TCP/IP
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = INADDR_ANY;  
    serv_addr.sin_port = htons(port);

    // Asocia el socket al puerto e IP
    if (bind(sockfd, (SOCKADDR*) &serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) 
    {
        error("ERROR doing binding");
    }
   
    return sockfd;
}

/**
 * Generates a connection
*/
SOCKET Server::socketListen(SOCKET sockfd)
{
    SOCKET newsockfd;
    struct sockaddr_in cli_addr;

    // Nos quedamos escuchando en el socket. Y genera una cola de 3 elemento
    if (listen(sockfd, 1) == SOCKET_ERROR)
    {
        error("ERROR in listen()");
    }

    // El comando accept() es el que realmente acepta una conexion de un cliente y hace el three-way-Handshake
    int clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd == INVALID_SOCKET) 
    {
        error("ERROR in accept()");
    }

    return newsockfd;
}

/*
 * Starts a 'conversation' with a clientfd SOCKET, when time elapsed is greater than timeout (ms), exit.
 * <param name="timeout">  Max time in miliseconds.</param>
*/
int Server::socketTurn(SOCKET clientfd, vector<Soldier> soldiers, int timeout)
{
    char buffer[256];
    string sendbuf, action;
    json json, json_body;
    int n;
    Utils utils;

    sendbuf = this->soldiersJson(soldiers).dump() + "\n";
    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);

    auto start_time = chrono::high_resolution_clock::now();

    auto end_time = chrono::high_resolution_clock::now();

    auto time_span = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);
 
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        sendbuf.clear();      
        json.clear();

        n = recv(clientfd, buffer, sizeof(buffer), 0);
        if (n < 0) error("ERROR reading socket");

        end_time = chrono::high_resolution_clock::now();
        time_span = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

        if (strcmp(buffer,"DONE\n") == 0)
        {
            break;
        }
           
        if (time_span.count() >= timeout)
        {
            sendbuf = "END\n";
            send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            n = recv(clientfd, buffer, sizeof(buffer), 0);
            if (n < 0) error("ERROR reading socket");

            break;
        }
        
        action = buffer;

        json_body = json::parse(action);

        if (! json_body["action"].is_null())
        {
            action = json_body["action"];

            if (action == "move")
            {
                int direction = 0;
                
                if (json_body["direction"] == "up") {
                    direction = Direction::up;
                } else if (json_body["direction"] == "down") {
                    direction = Direction::down;
                } else if (json_body["direction"] == "left") {
                    direction = Direction::left;
                } else if (json_body["direction"] == "right") {
                    direction = Direction::right;
                }

                if (direction == 0)
                {
                    json["Error"] = "Wrong direction or direction not passed.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (json_body["index"].is_null())
                {
                    json["Error"] = "index not found.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }
                
                int index = json_body["index"];

                Utils utils;
                auto soldiers = utils.getSoldiersFromSql();

                if(index < 0 || index >= soldiers.size()) {
                    json["Error"] = "Index: " + to_string(index) + " is out of bounds";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (soldiers.at(index).getTeam() == "TeamA")
                {
                    if (clientfd != this->config.getTeamASocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                } else {
                    if (clientfd != this->config.getTeamBSocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                }

                try{
                    soldiers.at(index).move(Direction(direction));
                } catch(const exception& e) {
                    json["Error"] = e.what();
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                json = this->soldierJson(soldiers.at(index));

                sendbuf = json.dump();                
                send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            }
            else if (action == "create")
            {
                if (json_body["index"].is_null())
                {
                    json["Error"] = "index not found.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }
                
                int index = json_body["index"];

                Utils utils;
                auto soldiers = utils.getSoldiersFromSql();

                if(index < 0 || index >= soldiers.size()) {
                    json["Error"] = "Index: " + to_string(index) + " is out of bounds";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (soldiers.at(index).getTeam() == "TeamA")
                {
                    if (clientfd != this->config.getTeamASocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                } else {
                    if (clientfd != this->config.getTeamBSocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                }

                try{
                    if (json_body["soldier"] == "Helicopter") {
                        soldiers.at(index).createHelicopter();
                    } else if (json_body["soldier"] == "Healer") {
                        soldiers.at(index).createHealer();
                    } else if (json_body["soldier"] == "Attacker") {
                        soldiers.at(index).createAttacker();
                    } else if (json_body["soldier"] == "Tank") {
                        soldiers.at(index).createTank();
                    }else {
                        json["Error"] = (string) json_body["soldier"] + " Is not a valid soldier.";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }

                } catch(const exception& e) {
                    json["Error"] = (string) e.what();
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                soldiers = utils.getSoldiersFromSql();

                json = this->soldierJson(soldiers.at(index));
                
                sendbuf = json.dump();
                send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            }
            else if (action == "attack")
            {
                if (json_body["posX"].is_null() || json_body["posY"].is_null()) {
                    json["Error"] = "Wrong pos.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (json_body["index"].is_null())
                {
                    json["Error"] = "index not found.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                int index = json_body["index"];

                Utils utils;
                auto soldiers = utils.getSoldiersFromSql();

                if(index < 0 || index >= soldiers.size()) {
                    json["Error"] = "Index: " + to_string(index) + " is out of bounds";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (soldiers.at(index).getTeam() == "TeamA")
                {
                    if (clientfd != this->config.getTeamASocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                } else {
                    if (clientfd != this->config.getTeamBSocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                }

                Location loc(json_body["posX"], json_body["posY"]);

                try{
                    soldiers.at(index).attack(loc);
                } catch(const exception& e) {
                    json["Error"] = e.what();
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                json = this->soldierJson(soldiers.at(index));

                sendbuf = json.dump();
                send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            }
            else if (action == "heal")
            {
                if (json_body["posX"].is_null() || json_body["posY"].is_null()) {
                    json["Error"] = "Wrong pos.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (json_body["index"].is_null())
                {
                    json["Error"] = "index not found.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                int index = json_body["index"];

                Utils utils;
                auto soldiers = utils.getSoldiersFromSql();

                if(index < 0 || index >= soldiers.size()) {
                    json["Error"] = "Index: " + to_string(index) + " is out of bounds";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (soldiers.at(index).getTeam() == "TeamA")
                {
                    if (clientfd != this->config.getTeamASocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                } else {
                    if (clientfd != this->config.getTeamBSocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                }

                Location loc(json_body["posX"], json_body["posY"]);

                try{
                    soldiers.at(index).heal(loc);
                } catch(const exception& e) {
                    json["Error"] = e.what();
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                json = this->soldierJson(soldiers.at(index));

                sendbuf = json.dump();
                send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            }
            else if (action == "getNearTiles")
            {
                if (json_body["index"].is_null())
                {
                    json["Error"] = "index not found.";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }
                
                int index = json_body["index"];

                Utils utils;
                auto soldiers = utils.getSoldiersFromSql();

                if(index < 0 || index >= soldiers.size()) {
                    json["Error"] = "Index: " + to_string(index) + " is out of bounds";
                    sendbuf = json.dump();
                    send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                    break;
                }

                if (soldiers.at(index).getTeam() == "TeamA")
                {
                    if (clientfd != this->config.getTeamASocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                } else {
                    if (clientfd != this->config.getTeamBSocket())
                    {
                        json["Error"] = "Can not control other team soldiers";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }
                }

                auto tiles = soldiers.at(index).getNearTiles();

                for(auto tile : tiles) {
                    string type = "";

                    switch (tile.getType()) {
                        case Tile::earth:
                            type = "earth";
                            break;

                        case Tile::water:
                            type = "water";
                            break;

                        case Tile::lava:
                            type = "lava";
                            break;
                    }

                    if ("" == type)
                    {
                        json["Error"] = "Type " + to_string(tile.getType()) + "not defined";
                        sendbuf = json.dump();
                        send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
                        break;
                    }

                    bool enemy = false;

                    if (tile.getOccupied() && soldiers.at(tile.getSoldierIndex()).getTeam() != soldiers.at(index).getTeam())
                    {
                        enemy = true;
                    }

                    nlohmann::json jsonTile{
                        {"posX", tile.getPos().x,},
                        {"posY", tile.getPos().y},
                        {"occupied", tile.getOccupied()},
                        {"enemy", enemy},
                        {"type", type}
                    };
                    
                    json["nearTiles"].push_back(jsonTile);
                }

                sendbuf = json.dump();
                send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            }
        }
        else
        {
            sendbuf = "END\n";
            cout << "END: not action in json" << endl;
            send(clientfd, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
            n = recv(clientfd, buffer, sizeof(buffer), 0);
            if (n < 0) error("ERROR reading socket while");

            break;
        }

        this->map.setSoldiers(utils.getSoldiersFromSql());
        this->map.setTiles(utils.getTilesFromSql());
    }

    return 0; 
}