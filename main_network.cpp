//
// Created by pospelov on 18.04.2020.
//
#include <iostream>
#include <string>

#include "draw_smf.hpp"
#include "game.hpp"
#include "Network.hpp"

int main(int argc, char **argv) {
  Game_state game_state;

  sf::RenderWindow window;
  window.setFramerateLimit(60);
  window.create(sf::VideoMode(1280, 720), "Chess");
  board_cell past_cell = {0, -1};

  if (argc < 2) {
    return 1;
  }

  Network network;

  number_of_player me = FIRST;

  std::string flex = argv[1];
  if (flex == "-1") {
    network.setup_server();
    me = FIRST;
  } else {
    network.connect_to_player(sf::IpAddress(flex));
    me = SECOND;
  }

  while (window.isOpen()) {
    sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    std::list<sf::RectangleShape> rendrer_list;

    sf::Event event;

    window.clear();
    draw_background(rendrer_list);
    draw_table(rendrer_list, game_state);
    draw_possible(rendrer_list, game_state, past_cell);

    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (game_state.who_moves() == me) {
        if (event.type == sf::Event::MouseButtonPressed) {
          if (event.mouseButton.button == sf::Mouse::Left) {
            int count = -1;
            int count1 = -1;
            for (auto &elem : rendrer_list) {
              if ((elem.getPosition() < pos) &&
                  (elem.getPosition() + elem.getSize()) > pos) {
                count = count1;
              }
              count1++;
              if (count1 > 63)
                break;
            }

            board_cell current_cell = {count / 8, count % 8};

            if (game_state.check_move(game_state.who_moves(), past_cell, current_cell)) {
              std::cerr << "Move: " << network.send_move(past_cell, current_cell) << "\n";
              game_state.move(game_state.who_moves(), past_cell, current_cell);
            }

            past_cell = current_cell;
          }
        }
      } else {
        board_cell enemy_past_cell, enemy_current_cell;
        if (network.get_enemy_move(enemy_past_cell, enemy_current_cell)) {
          std::cerr << "enemy\n";
          game_state.move(game_state.who_moves(), enemy_past_cell, enemy_current_cell);
        }
        past_cell = {0, -1};
      }
    }

    for (auto &elem : rendrer_list) {
      window.draw(elem);
    }

    window.display();
  }

  return 0;
}

/*

int main() {
  Ui ui;
  Model model;
  std::thread([] {
    while (true) {
      ui.draw();
    }
  });
  std::thread([] {
    while (true) {
      update_logic();
    }
  });
  std::thread([] {

  });
}*/
