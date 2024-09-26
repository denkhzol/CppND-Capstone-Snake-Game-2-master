#ifndef CONTROLLER_H
#define CONTROLLER_H

#include<thread>
#include "snake.h"
#include "bot.h"

class Controller {
 public:
  void HandleInput(bool &running, Snake &snake, std::thread &th1) const;

  void HandleInput(bool &running, Snake &snake, Bot &bot_player, std::thread &th1) const;

  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
};


#endif