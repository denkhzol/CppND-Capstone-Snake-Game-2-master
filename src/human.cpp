#include "human.h"

Human::Human() : _prev_score(0) {}
Human::Human(string name) : _name(name), _prev_score(0) {}
Human::Human(int prev_score, string name) : _prev_score(prev_score), _name(name) { _score = 0; }

Human::Human(const Human& other) : Player(other), _name(other._name), _prev_score(other._prev_score) {
    std::cout << "The Copy Constructor of Human is called " <<std::endl;
}

void Human::SetName(string name) { _name = name; }
string Human::GetName() { return _name; }

void Human::SetPrevScore(int prev_score) { _prev_score = prev_score; }
int Human::GetPrevScore() { return _prev_score; }