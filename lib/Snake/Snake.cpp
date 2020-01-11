#include "Snake.h"
#include "freertos/task.h"
#include <PS4Controller.h>

const Direction Direction::Up(0, 1);
const Direction Direction::Down(0, -1);
const Direction Direction::Left(-1, 0);
const Direction Direction::Right(1, 0);
const Direction Direction::UpRight(1, 1);
const Direction Direction::UpLeft(-1, 1);
const Direction Direction::DownLeft(-1, -1);
const Direction Direction::DownRight(1, -1);

Fruit create_random_fruit(const GameBoard& game_board, const std::vector<Fruit>& fruits, const Snake& snake) {
    return Fruit(Position(game_board.width/2, game_board.height/2));
}


void snake_game_task(void*) {

    uint32_t refresh_interval = 20;
    Direction dir(1, 0);
    Direction old_dir(0,0);
    GameBoard game_board (30, 10 , true, true, true, false);
    Snake snake(Position(game_board.width/2, game_board.height/2));
    std::vector<Fruit> fruits;

    while (true) {

        // get new direction
        old_dir = dir;
        if (PS4.isConnected()) {

            if (PS4.data.button.up) { // up
                Serial.println("Up Button");
                dir = Direction::Up;
            }
            else if (PS4.data.button.down) { // down
                Serial.println("Down Button");
                dir = Direction::Down;
            }
            else if (PS4.data.button.left) { // left
                Serial.println("Left Button");
                dir = Direction::Left;
            }
            else if (PS4.data.button.right) { // right
                Serial.println("Right Button");
                dir = Direction::Right;
            }
            else if (PS4.data.button.upright) { // upright
                Serial.println("Up Right Button");
                dir = Direction::UpRight;
            }
            else if (PS4.data.button.upleft) { // upleft
                Serial.println("Up Left Button");
                dir = Direction::UpLeft;
            }
            else if (PS4.data.button.downleft) { // downleft
                Serial.println("Down Left Button");
                dir = Direction::DownLeft;
            }
            else if (PS4.data.button.downright) { // downright
                Serial.println("Down Right Button");
                dir = Direction::DownRight;
            }
            else { // no new movement input
                // nothing to do
            }

        }

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
                snake.head().x = (snake.head().x + game_board.width) % game_board.width;
            }
            else {
                Serial.println("x - Out of gameboard");
                break;
            }
        }
        if (snake.head().y < 0 || snake.head().y >= game_board.height) {
            if (game_board.loop_y) {
                snake.head().y = (snake.head().y + game_board.height) % game_board.height;
            }
            else {
                Serial.println("y - Out of gameboard");
                break;
            }
        }


        // check if snake bites itself
        const auto bite_check = snake.is_biting_itself();
        if (bite_check.first) {
            snake.bite_off_tail(bite_check.second);
        }

        // check if snake can eat fruits
        for (auto& fruit : fruits) {
            if (snake.head() == fruit.position) {
                snake.eat(fruit);

                // create new fruit
                fruit = create_random_fruit(game_board, fruits, snake);
            }
        }

        // Draw everything

        // sleep
        delay(refresh_interval);
    }

}