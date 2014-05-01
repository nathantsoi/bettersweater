
#include <FrequencyTimer2.h>
#include "man_pattern_loader.h"

// pattern_loader should have
// num_patterns = total number of patterns
// pattern_lengths = [] how long is the pattern at the index?
// pattern_offsets = [] offset to the start of each pattern. [0] == 0
// patterns = [y][x] so we can read them: y0 is top row, x0 is first column
// {
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// { 1,1,1,1,1,1,1,1 }
// }

// Bigger == slower
#define SLIDE_SPEED 60

// STATE
// current pattern index
int current_pattern = 0;
// the leftmost block we've scrolled to
int current_scroll_offset = 0;

// Draw the values of this array to the screen
byte leds[8][8];


// PINS
byte col = 0;
// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[18]= {
  // placeholder
  -1,
  // matrix
  16, 12, 5, 4, 11, 17, 10, 9, 8, 23, 22, 7, 21, 6, 20, 19,
  // tree
  45
};
// col[xx] of leds = pin yy on led matrix
int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};
// row[xx] of leds = pin yy on led matrix
int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};
// trees are on this circuit
int tree = pins[17];
// toggle modes
int mode_btn = 25;


void setup() {
  Serial.begin(9600);

  // initialize the pushbutton pin as an input:
  pinMode(mode_btn, INPUT);
  // sets the pins as output
  for (int i = 1; i < 18; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // clear the screen
  setAllOff();
  setPattern(0);

  // Turn off toggling of pin 11
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(onTimer);
}

// RENDERING CODE
void onTimer() {
  noInterrupts();
  render();
  handleEvents();
  interrupts();
}

// SCROLLING CODE
void loop() {
  slidePattern(current_pattern, SLIDE_SPEED);
}

void render() {
  // $$$ SHOULD THIS BE LOW?
  digitalWrite(cols[col], HIGH);  // Turn whole previous column off
  col++;
  if (col == 8) { col = 0; }
  for (int row = 0; row < 8; row++) {
    // ON
    if (leds[col][7 - row] == 1) { digitalWrite(rows[row], HIGH); }
    else { digitalWrite(rows[row], LOW); }
  }
  // $$$ SHOULD THIS BE HIGH?
  // Turn whole column on at once (for equal lighting times)
  digitalWrite(cols[col], LOW);
}

void handleEvents() {
  checkModeButton();
}

int presses = 0;
int wasPressed = 0;
void checkModeButton() {
  // if the button is pressed
  if (digitalRead(mode_btn) == HIGH) {
    // and wasnt held down
    if (!wasPressed) {
      presses++;
      Serial.println(presses);
      setPattern(++current_pattern % (num_patterns-1));
      wasPressed = 1;
    }
  } else {
    wasPressed = 0;
  }
}

void setPattern(int pattern) {
  current_pattern = pattern;
  current_scroll_offset = 0;
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      leds[y][x] = patterns[pattern_offsets[current_pattern] + y][x];
    }
  }
}

void slidePattern(int pattern, int del) {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      leds[y][x] = patterns[y][(pattern_offsets[current_pattern] + current_scroll_offset + x) % pattern_lengths[current_pattern]];
    }
  }
  current_scroll_offset = ++current_scroll_offset % pattern_lengths[current_pattern];
  delay(del);
}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      leds[i][j] = 0;
    }
  }
}

void setAllOff() {
  // set up cols and rows
  for (int i = 0; i < 8; i++) {
    digitalWrite(cols[i], LOW);
  }

  for (int i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);
  }
}

int tree_toggle = 1;
int trees_on = 1;
void displayTree() {
  if (tree_toggle >= 1000) {
    if (trees_on) {
      //Serial.println("TREES OFF");
      digitalWrite(tree, LOW);
      trees_on = 0;
    } else {
      //Serial.println("TREES ON");
      digitalWrite(tree, HIGH);
      trees_on = 1;
    }
    tree_toggle = 0;
  }
  tree_toggle++;
}
