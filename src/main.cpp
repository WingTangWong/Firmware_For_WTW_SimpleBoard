// Firmware for Wing's SimpleBoard
// Author: Wing Tang Wong
// Copyright: ©2024 Wing Tang Wong
//
// * SimpleBoard was a project to test making a keypad and adding 
//   RGB led(s) as well as making the devices chainable for easy expansion
// 
// ** WARNING **
// Some seriously ugly code ahead. Using this as a scratchpad to test the functionality of the board and devices I hung off of it.
//
//
// Libraries Used:
// 
// * NeoPixelBus - https://github.com/Makuna/NeoPixelBus.git
// * Keypad - https://github.com/Chris--A/Keypad.git
// * U8g2 - https://github.com/olikraus/U8g2_Arduino.git


// For matrix keyboard control
#include <Keypad.h>

// For WS2812 RGB led controll
#include <NeoPixelBus.h>

// For buffer
#include <vector>

// For I2C OLED display
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define DISPLAY_POWER_PIN 23
/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/

// All Boards without Reset of the Display
// Using default SCL/SDA pins
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   

// 128 / 5 = 25.6 
// 24 * 5 = 120 + 8
// 0, 26, 32, 
// [24]+2+[24]+2+[24]+2+[24]+2+[24]
#define OLED_SEGMENT_WIDTH 24
#define OLED_SEGMENT_WIDTH_BORDER 2

// For the vertical, we have 64
// but would be nice to have division
// So height of 30px per button with a 2 pix border
#define OLED_SEGMENT_HEIGHT 30
#define OLED_SEGMENT_HEIGHT_BORDER 2



// Keypad Vars
const byte ROWS = 5; // Two rows
const byte COLS = 2; // Four columns
char keys[ROWS][COLS] = {
  {'0','1'},
  {'2','3'},
  {'4','5'},
  {'6','7'},
  {'8','9'},
};

// The ESP32-S with slightly odd pinout...
byte rowPins[ROWS] = {25,33,32,27,26};
// byte colPins[COLS] = {22,23};
byte colPins[COLS] = {5,18};

// void keypadEvent(KeypadEvent key); // declaration

Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// NeoPixel Vars
#define colorSaturation 128
const uint16_t PixelCount = 10; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const int fadeDuration = 450; // Time to fade in/out (in milliseconds)
const int updateInterval = 12; // Time between brightness updates (in milliseconds)

void simulateNeonEffect(uint16_t index, RgbColor color);


// Function to scale color brightness
RgbColor scaleBrightness(RgbColor color, float scale) {
  return RgbColor(
    static_cast<uint8_t>(color.R * scale),
    static_cast<uint8_t>(color.G * scale),
    static_cast<uint8_t>(color.B * scale)
  );
}


// Initialize the NeoPixelBus object
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

// Define RGB colors for retro look colors
RgbColor orangeNeon(255, 165, 0);
RgbColor redNeon(255, 0, 0);
RgbColor greenNeon(0, 255, 0);
RgbColor softWhiteNeon(255, 255, 224);

RgbColor softBlinkingOrange(255, 140, 0);
RgbColor softBlinkingGreen(144, 238, 144);
RgbColor softBlinkingRed(255, 99, 71);
RgbColor softBlinkingWhite(240, 248, 255);


// Define RGB colors for retro look colors
// RgbColor orangeNeon(96, 32, 0);
// RgbColor redNeon(96, 0, 0);
// RgbColor greenNeon(0, 96, 0);
// RgbColor softWhiteNeon(96, 96, 85);
// 
// RgbColor softBlinkingOrange(96, 64, 0);
// RgbColor softBlinkingGreen(64, 90, 64);
// RgbColor softBlinkingRed(96, 50, 40);
// RgbColor softBlinkingWhite(70, 80, 96);


// Guesstimate colors
RgbColor activeBright(64);
RgbColor activeDim(32);
RgbColor cmdModeBright(64,32,32);
RgbColor cmdModeDim(32,0,0);
RgbColor dataModeBright(32,32,64);
RgbColor dataModeDim(0,0,32);
RgbColor addrModeBright(32,64,32);
RgbColor addrModeDim(0,32,0);
RgbColor off(0);


// Function to blink an LED softly
void blinkSoftly(uint16_t index, RgbColor color) {
  static uint32_t lastUpdate = 0;
  static bool on = true;

  if (millis() - lastUpdate > 500) {  // 500ms for soft blinking interval
    on = !on;
    strip.SetPixelColor(index, on ? color : RgbColor(0, 0, 0)); // Toggle between color and off
    strip.Show();
    lastUpdate = millis();
  }
}


class ByteArrayChain {
private:
    std::vector<uint8_t> chain;
public:
    // Constructor to initialize the chain with a specific size
    ByteArrayChain(size_t size) : chain(size, 0) {
      // Make sure all values start at zero
      for(int i=0; i<size; i++) { insertAtHead('0'); };
    }

    // Function to insert a byte at the head and shift all values down
    void insertAtHead(uint8_t newByte) {
        if (chain.size() > 0) {
            // Shift all values down by 1
            for (size_t i = chain.size() - 1; i > 0; --i) {
                chain[i] = chain[i - 1];
            }
            // Insert the new byte at the head (index 0)
            chain[0] = newByte;
        } else {
          Serial.println("Array is empty, cannot insert at head.");
        }
    }

    // Function to delete the head and shift all values up
    void deleteHead() {
        if (!chain.empty()) {
            // Shift all values up by 1
            for (size_t i = 0; i < chain.size() - 1; ++i) {
                chain[i] = chain[i + 1];
            }
            // Set the last element to 0 (or you can pop_back to reduce the size)
            chain.back() = 0; // Maintain size but reset the last element
        } else {
            throw std::runtime_error("Array is empty, cannot delete head.");
        }
    }

    // Function to clear the array
    void clearChain() {
      std::fill(chain.begin(), chain.end(), 0);
    }

    // Function to read elements from the chain
    void readChain() const {
        if (!chain.empty()) {
            Serial.println("Byte Array Chain: ");
            for (const auto& byte : chain) {
                Serial.print(static_cast<char>(byte));
                Serial.print(" ");
            }
            Serial.println(" ");
        } else {
            Serial.println("Array is empty.");
        }
    }

    // Function to get the current size of the chain
    size_t size() const {
        return chain.size();
    }
};

ByteArrayChain byteChain(5);

// [24]+2+[24]+2+[24]+2+[24]+2+[24]
#define OLED_SEGMENT_WIDTH 24
#define OLED_SEGMENT_WIDTH_BORDER 2

// For the vertical, we have 64
// but would be nice to have division
// So height of 30px per button with a 2 pix border
#define OLED_SEGMENT_HEIGHT 30
#define OLED_SEGMENT_HEIGHT_BORDER 2

/*
class buttonDisplay {
  private:
    struct bdim {
      int x=0;
      int y=0;
      int w=0;
      int h=0;
      int state=0;
    };
    bdim buttons[ROWS * COLS];

  public:
    // Constructor
    buttonDisplay(int buttonWidth, int buttonHeight, int spacingX, int spacingY) {
      int index = 0;
      for (int row = 0; row < ROWS ; row++) {
        for (int col = 0; row < COLS ; col++) {
          buttons[index].x = row * (buttonWidth + spacingX);  // Set x position
          buttons[index].y = col * (buttonHeight + spacingY); // Set y position
          buttons[index].w = buttonWidth;  // Set button width
          buttons[index].h = buttonHeight; // Set button height
          buttons[index].state = 0;  
          index++;
        }
      }
    }

    void allOn();
    void allOff();
    void setOn(int button);
    void setOff(int button);
    void updateDisplay() {
      int index = 0;
      int xx,yy,ww,hh,ss;
      for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 5; col++) {

          xx = buttons[index].x;
          yy = buttons[index].y;
          ww = buttons[index].w;
          hh = buttons[index].h;
          ss = buttons[index].state;

          if (ss) {
          u8g2.drawBox(xx,yy,ww,hh); 
          } else {
          u8g2.drawFrame(xx,yy,ww,hh); 
          };

          index++;
        };
      };
      u8g2.sendBuffer();
    };
  private:
    int tvalue=0;
  };

buttonDisplay screen(24,30,2,2);
*/

void setup()
{
  // Setup Serial Debug/UART
  Serial.begin(115200);
  Serial.println("STARTUP");


  pinMode(DISPLAY_POWER_PIN, OUTPUT);
  digitalWrite(DISPLAY_POWER_PIN, HIGH);

  delay(100); // Give board a chance to power up

  // init OLED display
  u8g2.begin();
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);
  //u8g2.drawStr(0,10,"Hello World!");
  u8g2.drawBox(0,0,128,8);
  u8g2.drawBox(0,32,128,8);
  u8g2.sendBuffer();

  //screen.updateDisplay();
  // Initialize the strip
  strip.Begin();

  // Set initial colors for the first four LEDs
  strip.SetPixelColor(0, orangeNeon);   // Set first LED to Orange Neon
  strip.SetPixelColor(1, redNeon);      // Set second LED to Red Neon
  strip.SetPixelColor(2, greenNeon);    // Set third LED to Green Neon
  strip.SetPixelColor(3, softWhiteNeon);// Set fourth LED to Soft White Neon

  strip.Show(); // Update the strip to display colors

}


void display_keyboard() {
  Serial.println("1 3 5 7 [9:CLR]");
  Serial.println("0 2 4 6 [8:ENT]");
};

void display_context() {
  Serial.print("Buffer:[");
  byteChain.readChain();
  Serial.println(" ");
};

void loop()
{
  char customKey = customKeypad.getKey();

  if (customKey) {
    Serial.println(customKey);
    byteChain.insertAtHead(customKey);
    display_keyboard();
    Serial.print("Buffer: ");
    byteChain.readChain();
    Serial.println(" ");
    Serial.print("> ");
  };



    // Example for soft blinking effect on specific LEDs
  blinkSoftly(4, addrModeDim);  // Blink first LED with Soft Orange
  simulateNeonEffect(5, dataModeDim);     // Blink second LED with Soft Red
  blinkSoftly(6, cmdModeDim);   // Blink third LED with Soft Green
  simulateNeonEffect(7, cmdModeDim);   // Blink fourth LED with Soft White
  // strip.ClearTo(green100);
  // strip.Show();
  
}

// Function to simulate neon bulb lighting curve
void simulateNeonEffect(uint16_t index, RgbColor color) {
  static uint32_t lastUpdate = 0;
  static int step = 0; // Track animation state (up/down)
  static bool increasing = true; // Track if we are increasing brightness

  uint32_t currentMillis = millis();

  if (currentMillis - lastUpdate > updateInterval) {
    lastUpdate = currentMillis;

    float brightness = (float)step / (fadeDuration / updateInterval);

    // Adjust brightness curve to mimic neon on/off (nonlinear)
    float curve = brightness * brightness; // Squared curve for slower start/finish

    // Set color with scaled brightness
    strip.SetPixelColor(index, scaleBrightness(color, curve));
    strip.Show();

    // Adjust step and direction for smooth on/off cycle
    if (increasing) {
      step++;
      if (step >= (fadeDuration / updateInterval)) {
        step = (fadeDuration / updateInterval);
        increasing = false; // Start fading out
      }
    } else {
      step--;
      if (step <= 0) {
        step = 0;
        increasing = true; // Start brightening up again
      }
    }
  }
}
