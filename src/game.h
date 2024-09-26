#ifndef GAME_H
#define GAME_H

#include "human.h"
#include "bot.h"

#include <random>
#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "human.h"

enum PlayerType{
    _HUMAN = 1,
    _BOT = 2,
    _START_BUTTON = 3
  };

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);

  int GetScore() const;
  int GetSize() const;

  void SetPlayerType(int8_t player_type);
  int8_t GetPlayerType() const;

  void SetTmpPlayerName(std::string &&tmp_name);
  std::string GetTmpPlayerName();

  std::string GetPlayersFilename() const;

  Snake GetSnake();
  void PlaceAsteroid();

  // Write or update player's history to the file
  void SaveToFile();
  
 private:
  Snake snake;
  SDL_Point food;
  SDL_Point asteroid;

  // Player type: 1 - human, 2 - bot
  int8_t _player_type{PlayerType::_HUMAN};

  static const std::string players_filepath;

  std::string _tmp_player_name = "";

  std::shared_ptr<std::vector<Human>> prev_currunt_players;

  Human current_player;

  Bot bot_player;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;


  int score{0};

  void PlaceFood();
  void Update();

  // Read a player's history from a file if it exists
  std::shared_ptr<std::vector<Human>> LoadFromFile();

  void PrintLoadedPlayersHistory();

  Human FindAndAddCurrentPlayerHistory(const string& player_name);

  void UpdateCurrentPlayerInfoInHistory();

};

#endif