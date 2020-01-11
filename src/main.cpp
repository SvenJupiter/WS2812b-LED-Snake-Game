#include <Arduino.h>
#include <PS4Controller.h>

#include "Ringbuffer.h"

#include <FastLED.h>        // https://github.com/FastLED/FastLED
#include <LEDMatrix.h>      // https://github.com/Jorgen-VikingGod/LEDMatrix

#include "freertos/task.h"

// Change the next defines to match your matrix type and size
#define DATA_PIN            4

#define COLOR_ORDER         GRB
#define CHIPSET             WS2812B

// initial matrix layout (to get led strip index by x/y)
#define MATRIX_WIDTH        30
#define MATRIX_HEIGHT       10
#define MATRIX_TYPE         HORIZONTAL_ZIGZAG_MATRIX
#define MATRIX_SIZE         (MATRIX_WIDTH*MATRIX_HEIGHT)
#define NUMPIXELS           MATRIX_SIZE
#define BRIGHTNESS          32

// create our matrix based on matrix definition
cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> leds;

// MTX_MATRIX_TOP
// MTX_MATRIX_RIGH

uint8_t hue;
int16_t counter;


// BT-MAC-Address of KAI_HP_PC
#define HP_PC_BT_MAC "5C:F3:70:90:12:EB"

// BT-MAC-Address of Xperia XZ Smartphone
#define SMARTPHONE_BT_MAC "84:C7:EA:B1:11:AB"

void ps4_controller_setup();
void ps4_controller_test();
void ps4_task(void*);

void leds_setup();
void leds_test();
void led_task(void*);

Ringbuffer<int> buf = {1, 2, 3, 4, 5, 6, 7, 8};

TaskHandle_t ps4_task_handle = nullptr;
TaskHandle_t led_task_handle = nullptr;

void setup() {
    // put your setup code here, to run once:

    Serial.begin(115200); delay(1000);
    Serial.println("Ready.");

    // ps4_controller_setup();
    // leds_setup();

    xTaskCreatePinnedToCore(ps4_task, "PS4-Task", 8192, nullptr, 4, &ps4_task_handle, 0);
    xTaskCreatePinnedToCore(led_task, "LED-Task", 8192, nullptr, 4, &led_task_handle, 1);

}

void loop() {
    // put your main code here, to run repeatedly:

    // ps4_controller_test();
    // leds_test();
    while (true) { vTaskDelay(portMAX_DELAY); }

}



void ps4_controller_setup() {
    // put your setup code here, to run once:

    // Pair your Controller with your smartphone and use its BT-MAC-Address here
    PS4.begin(SMARTPHONE_BT_MAC); 

    while (!PS4.isConnected()) { 
        Serial.println("Connecting to PS4 Controller..."); 
        delay(1000);
    }
    Serial.println("PS4 Controller Connected!");

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
    FastLED.addLeds<CHIPSET, DATA_PIN, COLOR_ORDER>(leds[0], leds.Size()).setCorrection(TypicalSMD5050);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear(true);
    delay(500);

    FastLED.showColor(CRGB::Red);
    delay(1000);

    FastLED.showColor(CRGB::Lime);
    delay(1000);

    FastLED.showColor(CRGB::Blue);
    delay(1000);

    FastLED.showColor(CRGB::White);
    delay(1000);

    FastLED.clear(true);

    hue = 0;
    counter = 0;

    // for (uint32_t j = 0; j < leds.Height(); ++j) {
    //     for (uint32_t i = 0; i < leds.Width(); ++i) {
    //         leds(i, j) = CRGB::Green;
    //         FastLED.show();
    //         delay(250);
    //     }
    // }

    delay(5000);
    FastLED.clear(true);
}

void leds_test()
{
    int16_t x, y;
    uint8_t h;

    FastLED.clear();

    h = hue;
    if (counter < 1125)
    {
        // ** Fill LED's with diagonal stripes
        for (x=0; x<(leds.Width()+leds.Height()); ++x)
        {
            leds.DrawLine(x - leds.Height(), leds.Height() - 1, x, 0, CHSV(h, 255, 255));
            h+=16;
        }
    }
    else
    {
        // ** Fill LED's with horizontal stripes
        for (y=0; y<leds.Height(); ++y)
        {
            leds.DrawLine(0, y, leds.Width() - 1, y, CHSV(h, 255, 255));
            h+=16;
        }
    }
    hue+=4;

    if (counter < 125)
        ;
    else if (counter < 375)
        leds.HorizontalMirror();
    else if (counter < 625)
        leds.VerticalMirror();
    else if (counter < 875)
        leds.QuadrantMirror();
    else if (counter < 1125)
        leds.QuadrantRotateMirror();
    else if (counter < 1250)
        ;
    else if (counter < 1500)
        leds.TriangleTopMirror();
    else if (counter < 1750)
        leds.TriangleBottomMirror();
    else if (counter < 2000)
        leds.QuadrantTopTriangleMirror();
    else if (counter < 2250)
        leds.QuadrantBottomTriangleMirror();

    counter++;
    if (counter >= 2250)
        counter = 0;
    FastLED.show();
    delay(5);
}

void led_task(void*) {

    leds_setup();

    while (true) {
        leds_test();
    }
}
