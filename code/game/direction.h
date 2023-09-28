#ifndef DIRECTION_H
#define DIRECTION_H

class Direction
{
private:
    int direction;
public:
    Direction(int dir_) : direction(dir_) {};

    static int const up = 1;

    static int const right = 2;
    
    static int const down = 3;
    
    static int const left = 4;

    int getDirection() const { return direction; }
    void setDirection(int direction_) { direction = direction_; }
};

#endif // DIRECTION_H
