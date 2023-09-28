#include "setUp.h"

using namespace std;

Config SetUp::setArgs(int argc, char* const *argv)
{
    Config config;
    option long_opts[] = {
        {"ip", required_argument, 0, 'i'},
        {"help", no_argument, 0, 'h'},
    };
    const char* const short_opts = "r::h";

    for (;;)
    {
        auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt) {
            break;
        }    

        switch (opt)
        {
        case 'i':
            try
            {
                config.setIp(optarg);
            }
            catch(const std::exception& e)
            {
                std::cerr << "ip <ip addr> is compulsory" << '\n';
                help();
            }
            
            break;
        case 'h':
        case '?':
        default:
            help();
            break;
        }
    }

    return config;
}

void SetUp::help()
{
    cout <<
        "Usage:\n"
        "--ip           <ip_addr>:      Ip address in which you want the server, should be your ip (0.0.0.0 by default)\n"
        "--help:                        Show help\n";

    exit(1);
}

int main(int argc, char* const *argv)
{
    SetUp setUp;
    Config config (setUp.setArgs(argc, argv));

    Utils utils;
    utils.create_user_table();

    vector<Tile> map;
    Dashboard dashboard(map);
    shared_ptr<Server> svr = make_shared<Server>(dashboard);
    thread th1([&svr, config]{svr->startServer(config.getIp());});

    th1.join();
    return 0;
}