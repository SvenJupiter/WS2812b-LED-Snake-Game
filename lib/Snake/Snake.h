#pragma once

#include "stdint.h"
#include "FastLED.h"
#include "Ringbuffer.h"
#include "LedMatrix.h"
#include "Game.h"

namespace SnakeGame {

class Fruit {


    public:

        enum Type : int32_t {
            Normal_Type = 0, // Lets Snake grow
            SpeedBoost_Type = 1, // Make Snake faster for a few seconds
            SlowDown_Type = 2, // Make Snake slower for a few seconds
            Ghost_Type = 3, // Snake can pass over its body without biting it off
            Rainbow_Type = 4, // 
        };

        enum Color : int32_t {
            Normal_Color = CRGB::Blue,
            SpeedBoost_Color = CRGB::Red,
            SlowDown_Color = CRGB::Green,
            Ghost_Color = CRGB::White,
            Rainbow_Color = CRGB::Violet,
        };

    public:
        Game::Position position;
        Type type;
        CRGB color;

    Fruit(const Game::Position& pos, const Type& fruit_type = Normal_Type, const CRGB& Color = CRGB::Blue): position(pos), type(fruit_type), color(Color) {}

    Fruit(const Fruit& other): position(other.position), type(other.type), color(other.color) {}

    void operator=(const Fruit& other) {
        if (this != &other) {
            this->position = other.position;
            this->type = other.type;
            this->color = other.color;
        }
    }

    // Needed to put Fruits in a set (order doesn't matter in this case)
    bool operator<(const Fruit& other) const { return false; }
};
typedef std::vector<Fruit> FruitList;


class Snake {

    public:

        // int32_t length;

        Ringbuffer<Game::Position> body;
        CRGB head_color;
        CRGB body_base_color;
        uint8_t length_color_modifier;
        uint8_t time_color_modifier;

    public:

        Snake(const Game::Position initial_pos, const uint32_t initial_length = 5): body(initial_length, initial_pos), head_color(CRGB::Red), body_base_color(CRGB::Green) {}


        Game::Position& head() { return this->body.front(); }
        const Game::Position& head() const { return this->body.front(); }
        const Game::Position& tail() const { return this->body.back(); }
        Game::Position& tail() { return this->body.back(); }
        int32_t length() const { return this->body.size(); }

        void grow() {
            this->body.insert(this->body.end(), this->body.back());
        }

        void eat(const Fruit& fruit) {
            this->grow();
        }

        int32_t bite_off_tail(const Ringbuffer<Game::Position>::const_iterator& bite_mark) {
            this->body.erase(bite_mark, this->body.end());
            return this->length();
        }

        void move(const Game::Position& new_head_position) {
            this->body.push_front(new_head_position);
        }

        std::pair<bool, Ringbuffer<Game::Position>::const_iterator> is_biting_itself() const {

            // for every body part
            for (auto body_part = this->body.begin()+1; body_part != this->body.end(); ++body_part) {

                // check if position of body_part is the same as the position of the head
                if (this->head() == *body_part) {
                    return std::pair<bool, Ringbuffer<Game::Position>::const_iterator>(true, body_part);
                }
            }

            return std::pair<bool, Ringbuffer<Game::Position>::const_iterator>(false, this->body.end());
        }

    


};




Game::Direction get_direction_from_game_ai(const Game::GameBoard& game_board, const FruitList& fruits, const Snake& snake);

Fruit create_random_fruit(const Game::GameBoard& game_board, const FruitList& fruits, const Snake& snake);

void draw(LedMatrix& led_matrix, const Game::GameBoard& game_board, const FruitList& fruits, const Snake& snake, const bool write_to_leds = true);

void game_task(void*);

}; // namespace Snake