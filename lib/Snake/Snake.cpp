#include <Arduino.h>
#include "Snake.h"
#include "freertos/task.h"
#include <PS4Controller.h>
#include <map>
#include <algorithm>


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
    const Fruit fruit(Position(random(game_board.width), random(game_board.height)), Fruit::Normal_Type, CRGB::Orange /*CRGB::OrangeRed*/);
    printf("New Fruit at (%d/%d)\n", fruit.position.y, fruit.position.x);
    return fruit;
}

Direction get_direction_from_ps4_control_pad() {

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

Direction get_direction_from_ps4_analog_stick(const bool left_stick, const uint32_t magnitude_thershold) {

    if (PS4.isConnected()) {

        // const bool movement = ( \
        //     PS4.event.analog_move.stick.lx || PS4.event.analog_move.stick.ly || \
        //     PS4.event.analog_move.stick.rx || PS4.event.analog_move.stick.ry \
        // );

        // Get analog values from analog stick
        const int32_t analog_x = (left_stick ? PS4.data.analog.stick.lx :  PS4.data.analog.stick.rx);
        const int32_t analog_y = (left_stick ? PS4.data.analog.stick.ly :  PS4.data.analog.stick.ry);

        // Compare magnitude
        const int32_t magnitude_squared = ((analog_x*analog_x) + (analog_y*analog_y));
        if (magnitude_thershold*magnitude_thershold > magnitude_squared) {
            return Direction::None;
        }

    
        #define comp(value) (((value)*(16777216L))/(40503782L))

        if (analog_y > 0 && abs(analog_x) <= comp(analog_y)) { // up
            Serial.println("Stick Up");
            return Direction::Up;
        }
        else if (analog_y < 0 && abs(analog_x) <= comp(-analog_y)) { // down
            Serial.println("Stick Down");
            return Direction::Down;
        }
        else if (analog_x < 0 && abs(analog_y) <= comp(-analog_x)) { // left
            Serial.println("Left Button");
            return Direction::Left;
        }
        else if (analog_x > 0 && abs(analog_y) <= comp(analog_x)) { // right
            Serial.println("Stick Right");
            return Direction::Right;
        }
        else if (analog_y > comp(analog_x) && analog_x > comp(analog_y)) { // upright
            Serial.println("Stick Up Right");
            return Direction::UpRight;
        }
        else if (analog_y > comp(-analog_x) && (-analog_x) > comp(analog_y)) { // upleft
            Serial.println("Stick Up Left");
            return Direction::UpLeft;
        }
        else if (analog_y < comp(analog_x) && analog_x < comp(analog_y)) { // downleft
            Serial.println("Stick Down Left");
            return Direction::DownLeft;
        }
        else if (analog_x > comp(-analog_y) && (-analog_y) > comp(analog_x)) { // downright
            Serial.println("Stick Down Right");
            return Direction::DownRight;
        }
    }
    // else
    return Direction::None;
}

Direction get_direction_from_ps4() {
    return get_direction_from_ps4_control_pad() + get_direction_from_ps4_analog_stick(true, 100);
}


Direction get_direction_from_game_ai(const GameBoard& game_board, const FruitList& fruits, const Snake& snake) {

    std::map<int32_t, Direction> results;
    // results.reserve(fruit.size());
    for (auto& fruit : fruits) {
        const int32_t dist = Position::distance_squared(fruit.position, snake.head());
        const Direction dir = Position::direction(snake.head(), fruit.position).normalize();
        results.emplace(dist, dir);
    }

    const auto min_res = std::min_element(results.begin(), results.end(), [](std::map<int32_t, Direction>::value_type a, std::map<int32_t, Direction>::value_type b)-> bool { return (a.first < b.first); });
    // printf("AI-Direction: Head = %s, dist = %d, dir = %s\n", snake.head().to_string().c_str(), min_res->first, min_res->second.to_string().c_str());
    return min_res->second;
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
    if (write_to_leds) { FastLED.show(); }

}

void snake_game_task(void* args) {

    // Check task arguments, if nullptr terminate task immediately
    if (args == nullptr) { vTaskDelete(nullptr); }
    
    LedMatrix* led_matrix = (LedMatrix*) args;
    uint32_t refresh_interval = 125;
    const int32_t idle_timeout_ms = 10000;
    int32_t idle_timer_ms = 0;
    Direction dir(1, 0);
    Direction old_dir(0,0);
    GameBoard game_board (30, 10 , true, true, false, false);
    Snake snake(Position(game_board.width/2, game_board.height/2));
    FruitList fruits;

    while (true) {

        // Place fruits on gameboard
        for (auto i = 0; i < 10; ++i) {
            // create new fruit
            fruits.push_back(create_random_fruit(game_board, fruits, snake));
        }

        // Draw everything
        Serial.println("Initilizing Gameboard...");
        draw(*led_matrix, game_board, fruits, snake);

        // // wait for game start
        // Serial.println("Press any direction to start");
        // while (get_direction_from_ps4() == Direction::None) { delay(refresh_interval); }

        // Game Loop
        TickType_t xPreviousWakeTime = xTaskGetTickCount();
        while (true) {

            // get new direction
            old_dir = dir;
            dir = get_direction_from_ps4();
            if (dir == Direction::None) {
                if (idle_timer_ms <= 0) { dir = get_direction_from_game_ai(game_board, fruits, snake); refresh_interval = 200; }
                else { idle_timer_ms -= refresh_interval; }
            }
            else { idle_timer_ms = idle_timeout_ms; refresh_interval = 125; }
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

            // Serial.print("Snake Head at pos. ("); 
            // Serial.print(snake.head().x, DEC);
            // Serial.print("|");
            // Serial.print(snake.head().y, DEC);
            // Serial.println(")");


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
                    // fruits.erase(fruit);

                    // create new fruit
                    fruit = create_random_fruit(game_board, fruits, snake);

                    // Snake can only eat one fruit at a time
                    break;
                }
            }



            // Draw everything
            draw(*led_matrix, game_board, fruits, snake, true);

            // sleep
            vTaskDelayUntil(&xPreviousWakeTime, pdMS_TO_TICKS(refresh_interval));
        }


        // delay after game ended
        while (get_direction_from_ps4() != Direction::None) { delay(refresh_interval); }
        delay (5000);

    }

}