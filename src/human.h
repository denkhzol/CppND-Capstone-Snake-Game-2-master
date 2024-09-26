#ifndef HUMAN_H
#define HUMAN_H

#include "player.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

using std::string;

class Human : public Player {
private:
    string _name;
    int _prev_score;
public:
    Human();
    Human(string name);
    Human(int prev_score, string name);
    Human(const Human& other);

    void SetName(string name);
    string GetName();
    void SetPrevScore(int score);
    int GetPrevScore();
};

#endif // HUMAN_H
