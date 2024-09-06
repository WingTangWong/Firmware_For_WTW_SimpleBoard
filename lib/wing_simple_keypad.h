#ifndef WING_SIMPLE_KEYPAD_H
#define WING_SIMPLE_KEYPAD_H

class WING_SIMPLE_KEYPAD
{
  public:
    // Constructor
    WING_SIMPLE_KEYPAD() {

    };

    // Destructor
    ~WING_SIMPLE_KEYPAD() {

    };

    // Public methods to perform conversion of positioning systems to POS
    void XY_to_POS(int x, int y, int & pos);
    void RowCol_to_POS(int col, int row, int & pos);
    void ButtonNumber_to_POS(int button_number, int & pos);

    // Public methods to perform conversion of POS to positioning systems
    void POS_to_XY (int pos, int & x, int & y);
    void POS_to_RowCol (int pos, int & row, int & col);
    void POS_to_ButtonNumber (int pos, int & button);

  private:





// The keypad is 2x5 for 10 RGB led positions
// Zigzag arrangement

// ```
// <pins> <pins>
// [01]   [03]   [05]   [07]   [09]
// [00]   [02]   [04]   [06]   [08]
// <pins>
// ```

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object
MyAnimationState animationState[AnimCount];
uint16_t frontPixel = 0;  // the front of the loop
RgbColor frontColor;  // the color at the front of the loop

int XY_to_POS(int x, int y) {
  /*
   *     x=0    x=1    x=2    x=3    x=4
    y=0 [01]   [03]   [05]   [07]   [09]
    y=1 [00]   [02]   [04]   [06]   [08]
   *
   */
  int pos = 0;
  if y == 0 {
    if x == 0 { pos = 1; };
    if x == 1 { pos = 3; };
    if x == 2 { pos = 5; };
    if x == 3 { pos = 7; };
    if x == 4 { pos = 9; };
  } else {
    if x == 0 { pos = 0; };
    if x == 1 { pos = 2; };
    if x == 2 { pos = 4; };
    if x == 3 { pos = 6; };
    if x == 4 { pos = 8; };
  };
  return pos;
};

int RowCol_to_POS(int col, int row) {
  /*
   *     col0   col1   col2   col3   col4
    row0 [01]   [03]   [05]   [07]   [09]
    row1 [00]   [02]   [04]   [06]   [08]
   *
   */
  int pos = 0;
  if row == 0 {
    if col == 0 { pos = 1; };
    if col == 1 { pos = 3; };
    if col == 2 { pos = 5; };
    if col == 3 { pos = 7; };
    if col == 4 { pos = 9; };
  } else {
    if col == 0 { pos = 0; };
    if col == 1 { pos = 2; };
    if col == 2 { pos = 4; };
    if col == 3 { pos = 6; };
    if col == 4 { pos = 8; };
  };
  return pos;
};

int ButtonNumber_to_POS(int button_number) {
  /*
    [01]   [02]   [03]   [04]   [05]
    [06]   [07]   [08]   [09]   [10]
   */
  int pos = 0;
  if button_number == 1 { pos = 1; };
  if button_number == 2 { pos = 3; };
  if button_number == 3 { pos = 5; };
  if button_number == 4 { pos = 7; };
  if button_number == 5 { pos = 9; };
  if button_number == 6 { pos = 0; };
  if button_number == 7 { pos = 2; };
  if button_number == 8 { pos = 4; };
  if button_number == 9 { pos = 8; };
  if button_number == 10 { pos = 10; };
  return pos;
};

struct key_state
{
  bool button_pressed  = false;
  bool led_on = false;
  int red = 0;
  int green = 0;
  int blue = 0;
};

key_state keypad[10];


void SetButtonLED_via_POS(keypad_state keys, int position, char * color) {
};

#endif
