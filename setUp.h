#ifndef MAIN_H
#define MAIN_H
#include <getopt.h>
#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include <fstream>
#include <sqlite3.h>

#include "server.h"
#include "config.h"
#include "utils.h"
#include "soldier.h"
#include "location.h"
#include "dashboard.h"
#include "tile.h"

class SetUp
{
public:
    Config setArgs(int argc, char* const *argv);
    void help();
};

#endif // MAIN_H