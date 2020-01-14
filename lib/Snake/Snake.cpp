#include <Arduino.h>
#include "Snake.h"
#include "freertos/task.h"
#include <PS4Controller.h>


const Direction Direction::Up(0, -1);
const Direction Direction::Down(0, 1);
const Direction Direction::Left(-1, 0);
const Direction Direction::Right(1, 0);
const Direction Direction::UpRight(1, -1);
const Direction Direction::UpLeft(-1, -1);
const Direction Direction::DownLeft(-1, 1);
const Direction Direction::DownRight(1, 1);
const Direction Direction::None(0, 0);

Fruit create_random_fruit(const GameBoard& game_board, const FruitList& fruits, const Snake& snake) {
    return Fruit(Position(game_board.width/2, game_board.height/2), Fruit::Normal_Type, CRGB::Yellow);
}

Direction get_direction_from_ps4() {

    if (PS4.isConnected()) {

        if (PS4.data.button.up) { // up
            Serial.println("Up Button");
            return Direction::Up;
        }
        else if (PS4.data.button.down) { // down
            Serial.println("Down Button");
            return Direction::Down;
        }
        else if (PS4.data.button.left) { // left
            Serial.println("Left Button");
            return Direction::Left;
        }
        else if (PS4.data.button.right) { // right
            Serial.println("Right Button");
            return Direction::Right;
        }
        else if (PS4.data.button.upright) { // upright
            Serial.println("Up Right Button");
            return Direction::UpRight;
        }
        else if (PS4.data.button.upleft) { // upleft
            Serial.println("Up Left Button");
            return Direction::UpLeft;
        }
        else if (PS4.data.button.downleft) { // downleft
            Serial.println("Down Left Button");
            return Direction::DownLeft;
        }
        else if (PS4.data.button.downright) { // downright
            Serial.println("Down Right Button");
            return Direction::DownRight;
        }
    }
    // else
    return Direction::None;
}


void draw(LedMatrix& led_matrix, const GameBoard& game_board, const FruitList& fruits, const Snake& snake, const bool write_to_leds) {


    // check if game_borad dimensions and matrix dimensions match
    if (led_matrix.width() != game_board.width || led_matrix.height() != game_board.height) {
        return;
    }

    // draw game_board
    for (uint32_t row = 0; row < game_board.height; ++row) {
        for (uint32_t column = 0; column < game_board.width; ++column) {
            led_matrix(row, column) = game_board.board_color;
        }
    }

    // draw fruits
    for (const auto& fruit : fruits) {
        led_matrix(fruit.position.y, fruit.position.x) = fruit.color;
    }

    // draw snake body
    for (const auto& body_part : snake.body) {
        led_matrix(body_part.y, body_part.x) = snake.body_base_color;
    }
    
    // draw snake head
    led_matrix(snake.head().y, snake.head().x) = snake.head_color;

    // push to matrix
    if (true || write_to_leds) { FastLED.show(); }

}

void snake_game_task(void* args) {

    // Check task arguments, if nullptr terminate task immediately
    if (args == nullptr) { vTaskDelete(nullptr); }
    
    LedMatrix* led_matrix = (LedMatrix*) args;
    uint32_t refresh_interval = 125;
    Direction dir(1, 0);
    Direction old_dir(0,0);
    GameBoard game_board (30, 10 , true, true, false, false);
    Snake snake(Position(game_board.width/2, game_board.height/2));
    FruitList fruits;

    while (true) {

        // Draw everything
        Serial.println("Initilizing Gameboard...");
        draw(*led_matrix, game_board, fruits, snake);

        // wait for game start
        Serial.println("Press any direction to start");
        while (get_direction_from_ps4() == Direction::None) { delay(refresh_interval); }

        // Game Loop
        while (true) {

            // get new direction
            old_dir = dir;
            dir = get_direction_from_ps4();
            if (dir == Direction::None) { dir = old_dir; }


            // invert movement if nessesary
            if (game_board.invert_x_movement) { dir.x *= (-1); }
            if (game_board.invert_y_movement) { dir.y *= (-1); }

            // we can't go back
            if (dir + old_dir == Direction(0,0)) { dir = old_dir; }

            // move snake
            snake.move(snake.head() + dir);

            // check if out of gameboard (loop if nesesary)
            if (snake.head().x < 0 || snake.head().x >= game_board.width) {
                if (game_board.loop_x) {
                    Serial.println("x - loopback");
                    snake.head().x = (snake.head().x + game_board.width) % game_board.width;
                }
                else {
                    Serial.println("x - Out of gameboard");
                    break;
                }
            }
            if (snake.head().y < 0 || snake.head().y >= game_board.height) {
                if (game_board.loop_y) {
                    Serial.println("y - loopback");
                    snake.head().y = (snake.head().y + game_board.height) % game_board.height;
                }
                else {
                    Serial.println("y - Out of gameboard");
                    break;
                }
            }

            Serial.print("Snake Head at pos. ("); 
            Serial.print(snake.head().x, DEC);
            Serial.print("|");
            Serial.print(snake.head().y, DEC);
            Serial.println(")");


            // check if snake bites itself
            Serial.println("Bite Check...");
            const auto bite_check = snake.is_biting_itself();
            if (snake.body.size() > 1 && bite_check.first) {
                Serial.println("Biting of Tail!");
                snake.bite_off_tail(bite_check.second);
            }

            // check if snake can eat fruits
            for (auto& fruit : fruits) {
                if (snake.head() == fruit.position) {
                    snake.eat(fruit);
                    fruits.erase(fruit);

                    // Snake can only eat one fruit at a time
                    break;
                }
            }

            // create new fruit
            fruits.insert(create_random_fruit(game_board, fruits, snake));

            // Draw everything
            draw(*led_matrix, game_board, fruits, snake);

            // sleep
            delay(refresh_interval);
        }


        // delay after game ended
        while (get_direction_from_ps4() != Direction::None) { delay(refresh_interval); }
        delay (5000);

    }

}