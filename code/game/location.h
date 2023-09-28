#ifndef LOCATION_H
#define LOCATION_H

struct Location
{
    int x;
    int y;
    Location() : x(0), y(0) {}
    Location(int x_, int y_) : x(x_), y(y_) {}
};

#endif // LOCATION_H
