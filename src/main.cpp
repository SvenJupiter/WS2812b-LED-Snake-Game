#include <Arduino.h>
#include <PS4Controller.h>

#include "Ringbuffer.h"

#include <FastLED.h>        // https://github.com/FastLED/FastLED
#include "LedMatrix.h"
#include <array>

#include "freertos/task.h"
#include "Snake.h"

// Change the next defines to match your matrix type and size
#define DATA_PIN            4

#define COLOR_ORDER         GRB
#define CHIPSET             WS2812B

// initial matrix layout (to get led strip index by x/y)
#define MATRIX_WIDTH            30
#define MATRIX_HEIGHT           10
#define MATRIX_WIRING_START     LedMatrix::TopRight
#define MATRIX_WIRING_PATTERN   LedMatrix::HorizontalZigZag
#define MATRIX_SIZE             (MATRIX_WIDTH*MATRIX_HEIGHT)
#define NUMPIXELS               MATRIX_SIZE
#define BRIGHTNESS              32

// create our matrix based on matrix definition
std::array<CRGB, MATRIX_SIZE> leds;
LedMatrix led_matrix(leds.data(), MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_WIRING_START, MATRIX_WIRING_PATTERN);


// BT-MAC-Address of Smartphone
#define SMARTPHONE_BT_MAC "84:C7:EA:B1:11:AB"

void ps4_controller_setup();
void ps4_controller_test();
void ps4_task(void*);

void leds_setup();

TaskHandle_t snake_task_handle = nullptr;

void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200); delay(1000);
    Serial.println("Ready.");

    ps4_controller_setup();
    leds_setup();

    // xTaskCreatePinnedToCore(ps4_task, "PS4-Task", 8192, nullptr, 4, &ps4_task_handle, 0);
    // xTaskCreatePinnedToCore(led_task, "LED-Task", 8192, nullptr, 4, &led_task_handle, 1);

    xTaskCreatePinnedToCore(SnakeGame::game_task, "Snake-Task", 2 * 8192, &led_matrix, 4, &snake_task_handle, 1);

}

void loop() {
    // put your main code here, to run repeatedly:

    while (true) { vTaskDelay(portMAX_DELAY); }

}



void ps4_controller_setup() {
    // put your setup code here, to run once:

    // Pair your Controller with your smartphone and use its BT-MAC-Address here
    PS4.begin(SMARTPHONE_BT_MAC); 

    // while (!PS4.isConnected()) { 
    //     Serial.println("Connecting to PS4 Controller..."); 
    //     delay(1000);
    // }
    // Serial.println("PS4 Controller Connected!");

}

void ps4_controller_test() {

  

    // Below has all accessible outputs from the controller
    if(PS4.isConnected()) {

        if ( PS4.data.button.up )
            Serial.println("Up Button");
        if ( PS4.data.button.down )
            Serial.println("Down Button");
        if ( PS4.data.button.left )
            Serial.println("Left Button");
        if ( PS4.data.button.right )
            Serial.println("Right Button");

        if ( PS4.data.button.upright )
            Serial.println("Up Right");
        if ( PS4.data.button.upleft )
            Serial.println("Up Left");
        if ( PS4.data.button.downleft )
            Serial.println("Down Left");
        if ( PS4.data.button.downright )
            Serial.println("Down Right");

        if ( PS4.data.button.triangle )
        Serial.println("Triangle Button");
        if ( PS4.data.button.circle )
        Serial.println("Circle Button");
        if ( PS4.data.button.cross )
            Serial.println("Cross Button");
        if ( PS4.data.button.square )
            Serial.println("Square Button");

        if ( PS4.data.button.l1 )
            Serial.println("l1 Button");
        if ( PS4.data.button.r1 )
            Serial.println("r1 Button");

        if ( PS4.data.button.l3 )
            Serial.println("l3 Button");
        if ( PS4.data.button.r3 )
            Serial.println("r3 Button");

        if ( PS4.data.button.share )
            Serial.println("Share Button");
        if ( PS4.data.button.options )
            Serial.println("Options Button");

        if ( PS4.data.button.ps )
            Serial.println("PS Button");
        if ( PS4.data.button.touchpad )
            Serial.println("Touch Pad Button");

        if ( PS4.data.button.l2 ) {
            Serial.print("l2 button at ");
            Serial.println(PS4.data.analog.button.l2, DEC);
        }
        if ( PS4.data.button.r2 ) {
            Serial.print("r2 button at ");
            Serial.println(PS4.data.analog.button.r2, DEC);
        }

        if ( PS4.event.analog_move.stick.lx ) {
            if (abs(PS4.data.analog.stick.lx) >= 100) {
                Serial.print("Left Stick x at ");
                Serial.println(PS4.data.analog.stick.lx, DEC);
            }
        }
        if ( PS4.event.analog_move.stick.ly ) {
            if (abs(PS4.data.analog.stick.ly) >= 100) {
                Serial.print("Left Stick y at ");
                Serial.println(PS4.data.analog.stick.ly, DEC);
            }
        }
        if ( PS4.event.analog_move.stick.rx ) {
            if (abs(PS4.data.analog.stick.rx) >= 100) {
                Serial.print("Right Stick x at ");
                Serial.println(PS4.data.analog.stick.rx, DEC);
            }
        }
        if ( PS4.event.analog_move.stick.ry ) {
            if (abs(PS4.data.analog.stick.ry) >= 100) {
                Serial.print("Right Stick y at ");
                Serial.println(PS4.data.analog.stick.ry, DEC);
            }
        }

        if (PS4.data.status.charging)
            Serial.println("The controller is charging");
        if (PS4.data.status.audio)
            Serial.println("The controller has headphones attached");
        if (PS4.data.status.mic)
            Serial.println("The controller has a mic attached");

        // Serial.print("Battey = ");
        // Serial.print(PS4.data.status.battery, DEC);
        // Serial.println(" / 16");

        // Serial.println();

    }
    else {
        Serial.println("Not Connected!");
        delay(1000);
    }

    // This delay is to make the Serial Print more human readable
    // Remove it when you're not trying to see the output
    delay(50);

}

void ps4_task(void*) {

    ps4_controller_setup();

    while (true) {
        ps4_controller_test();
    }
}

void leds_setup()
{
    Serial.println("Testing LEDs..."); 

    // initial LEDs
    FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds.data(), leds.size()).setCorrection(TypicalSMD5050);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear(true);
    delay(500);

    // FastLED.showColor(CRGB::Red);
    // delay(1000);

    // FastLED.showColor(CRGB::Lime);
    // delay(1000);

    // FastLED.showColor(CRGB::Blue);
    // delay(1000);

    // FastLED.showColor(CRGB::White);
    // delay(1000);

    // FastLED.clear(true);

    // // LedMatrix Test
    // for (uint32_t row = 0; row < led_matrix.height(); ++row) {
    //     for (uint32_t colum = 0; colum < led_matrix.width(); ++colum) {
    //         led_matrix(row, colum) = CRGB::Green;
    //         FastLED.show();
    //         delay(250);
    //     }
    // }

    // delay(5000);
    // FastLED.clear(true);
}

