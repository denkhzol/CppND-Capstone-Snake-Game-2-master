#include "game.h"
#include <iostream>
#include <thread>
#include <functional>
#include <future>
#include "SDL.h"
#include "controller.h"

const std::string Game::players_filepath = "./players.txt";

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
  PlaceAsteroid();
}

// Place a asteroid as a game obstacle randomly, then if snake over pass it, their score will be reduced.
void RandomAsteroid(std::promise<std::string> &&prms, Game *gm, bool &running, std::string message_to_thread){
  std::cout << "Place the asteroid randomly." << std::endl;
  
  prms.set_value("The first asteroid is in place, and it will lower your score.");

  while (gm->GetSnake().alive)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    
    std::cout<<message_to_thread<< std::endl;

    gm->PlaceAsteroid();

    std::mutex tmp_mtx;
    std::lock_guard<std::mutex> lck(tmp_mtx);
    if(!running)
      break;
  }
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  //Check if human or auto player
  if(GetPlayerType() == PlayerType::_HUMAN){
    prev_currunt_players = LoadFromFile();

    // Print loaded players' history
    PrintLoadedPlayersHistory();

    // Set a default name to player if no name is typed
    if(GetTmpPlayerName() == ""){
        SetTmpPlayerName("HumanPlayer");
    }

    current_player = FindAndAddCurrentPlayerHistory(GetTmpPlayerName());
    
    PrintLoadedPlayersHistory();
  } else if(GetPlayerType() == PlayerType::_BOT){
    bot_player = Bot();
  }

  // Asteroid
  std::promise<std::string> prms;
  std::future<std::string> ftr = prms.get_future();
  std::string message_to_thread = "placing an asteroid ... ";
 
  std::thread th_asteroid(RandomAsteroid, std::move(prms), this, std::ref(running), message_to_thread);
  
  // Update the window title and show message box
  renderer.UpdateWindowTitle(ftr.get());

  // Avoid data races
  std::mutex tmp_mtx;
  tmp_mtx.lock();
  while (running) {
    tmp_mtx.unlock();

    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    if(GetPlayerType() == PlayerType::_HUMAN)
      controller.HandleInput(running, snake, th_asteroid);

    // Bot might change direction for each 3000 frames
    else if (GetPlayerType() == PlayerType::_BOT && frame_count % 3000 == 0){
      controller.HandleInput(running, snake, bot_player, th_asteroid);
    }    

    Update();
    renderer.Render(snake, food, asteroid);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }

  tmp_mtx.lock();
  if(running = false){
   tmp_mtx.unlock();

    th_asteroid.join();

    // before finishing a game, store the players' information to the file
    SaveToFile();
    std::cout<<"Due to FALSE, the current player's history was overwritten into the file."<<std::endl;
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::PlaceAsteroid(){
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // asteroid.
    if (!snake.SnakeCell(x, y)) {
      asteroid.x = x;
      asteroid.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    // Update the player's score
    current_player.SetScore(score);
    std::cout << "Current Player: " << current_player.GetName() << ", Previous Score: " << current_player.GetPrevScore() << ", Score: " << current_player.GetScore() << std::endl;
    UpdateCurrentPlayerInfoInHistory();

    PlaceFood();

    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }

  // Check if there's asteroid over here
  if (asteroid.x == new_x && asteroid.y == new_y) {
    score--;

    //Update the player's score
    current_player.SetScore(score);
    std::cout << "Current Player: " << current_player.GetName() << ", Previous Score: " << current_player.GetPrevScore() << ", Score: " << current_player.GetScore() << std::endl;
    UpdateCurrentPlayerInfoInHistory();

    PlaceAsteroid();
    // Grow snake and increase speed.
    snake.ShrinkBody();
    snake.speed -= 0.02;
  }
}

// Write players' information {name, score} set to the file
void Game::SaveToFile() {
    std::ofstream file(players_filepath);
    if (file.is_open()) {
        PrintLoadedPlayersHistory();
        for (auto &human : *prev_currunt_players) {
            if(human.GetName() == current_player.GetName())
              file << "{" << human.GetName() << "," << human.GetScore() << "}" << std::endl;
            else
              file << "{" << human.GetName() << "," << human.GetPrevScore() << "}" << std::endl;
        }
        file.close();
        std::cout<<"The history file was updated successfully." << std::endl;
    } else {
        std::cerr << "Unable to open file for writing" << std::endl;
    }
}

// Load the players' information from file
std::shared_ptr<std::vector<Human>> Game::LoadFromFile() {
    std::ifstream file(players_filepath);
    
    std::shared_ptr<std::vector<Human>> humans = std::make_shared<std::vector<Human>>();

    if (!file.is_open()) {
        // File does not exist, create it
        std::ofstream outfile(players_filepath);
        if (!outfile) {
            std::cerr << "Unable to create file" << std::endl;
            return humans;
        }
        outfile.close();
        std::cout << "File created as it did not exist." << std::endl;
    } else {
        std::cout << "Loading information from the file." << std::endl;
        std::string line;
        while (getline(file, line)) {
            size_t start = line.find("{") + 1;
            size_t end = line.find("}");
            std::string content = line.substr(start, end - start);
            size_t first_comma = content.find(",");
            std::string name = content.substr(0, first_comma);
            int prev_score = std::stoi(content.substr(first_comma + 1));
            Human human(prev_score, name);
            humans->emplace_back(human);
        }
        file.close();
        std::cout << "The file was closed." << std::endl;
    }
  
    return humans;
}

void Game::PrintLoadedPlayersHistory() {
    for (auto &human : *prev_currunt_players) {
        std::cout << "Name: " << human.GetName() << ", Previous Score: " << human.GetPrevScore() << ", Score: " << human.GetScore() << std::endl;
    }
}

// Find a specific player
Human Game::FindAndAddCurrentPlayerHistory(const string& player_name) {
    bool isExisting = false;

    for (auto &human : *prev_currunt_players) {
        if (human.GetName() == player_name) {
            human.SetScore(0);
            return human;
        }
    }

    if(!isExisting){
      current_player.SetName(player_name);
      prev_currunt_players->emplace_back(current_player);
    }

    return Human(player_name);
}

// Update prev_currunt_players by current_player
void Game::UpdateCurrentPlayerInfoInHistory() {
    for (auto &human : *prev_currunt_players) {
        if (human.GetName() == current_player.GetName()) {
            human.SetScore(current_player.GetScore());
            break;
        }
    }
}

Snake Game::GetSnake(){return snake; }
int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

void Game::SetPlayerType(int8_t player_type) {_player_type = player_type;}
int8_t Game::GetPlayerType() const {return _player_type;}

void Game::SetTmpPlayerName(std::string &&tmp_name){_tmp_player_name = tmp_name;}
std::string Game::GetTmpPlayerName(){return _tmp_player_name;}
std::string Game::GetPlayersFilename() const {return players_filepath;}