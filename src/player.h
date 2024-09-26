#ifndef PLAYER_H
#define PLAYER_H

// Abstract class
class Player {
protected:
    int _score;
public:
    virtual void SetScore(int score);
    virtual int GetScore();
};

#endif // PLAYER_H
