
#include <FrequencyTimer2.h>
#include "letters.h"

// Bigger == slower
const int SLIDE_SPEED = 90;

const int numPatterns = 12;
byte patterns[numPatterns][8][8] = {
  M,E,R,R,Y,SPACE,TOPLEFT,BLOCK,ONE,TWO,THREE,FOUR
};

int pattern = 0;

int presses = 0;
int wasPressed = 0;

// PINS
byte col = 0;
byte leds[8][8];

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[18]= {
  // placeholder
  -1,
  // matrix
  16, 12, 5, 4, 11, 17, 10, 9, 8, 23, 22, 7, 21, 6, 20, 19,
  // tree
  45
};

int btn = 38;

int tree = pins[17];

// col[xx] of leds = pin yy on led matrix
int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[06], pins[11], pins[15], pins[16]};

// row[xx] of leds = pin yy on led matrix
int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};



void setup() {
  Serial.begin(9600); 
  
  // initialize the pushbutton pin as an input:
  pinMode(btn, INPUT);   
  
  // sets the pins as output
  for (int i = 1; i < 18; i++) {
    pinMode(pins[i], OUTPUT);
  }

  setAllOff();

  clearLeds();

  // Turn off toggling of pin 11
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);

  setPattern(pattern);
}

void loop() {
  // if the button is pressed
  if (digitalRead(btn) == HIGH) {
    if (!wasPressed) {
      presses++;
      Serial.println(presses);
      wasPressed = 1;
    }
  } else {
    wasPressed = 0;
  } 
  pattern = ++pattern % numPatterns;
  slidePattern(pattern, SLIDE_SPEED);
}

void clearLeds() {
  // Clear display array
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      leds[i][j] = 0;
    }
  }
}

void setPattern(int pattern) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      leds[i][j] = patterns[pattern][i][j];
    }
  }
}

void slidePattern(int pattern, int del) {
  for (int l = 0; l < 8; l++) {
    for (int i = 0; i < 7; i++) {
      for (int j = 0; j < 8; j++) {
        leds[j][i] = leds[j][i+1];
      }
    }
    for (int j = 0; j < 8; j++) {
      leds[j][7] = patterns[pattern][j][0 + l];
    }
    delay(del);
  }
}

int tree_toggle = 1;
int trees_on = 1;

int debugX, debugY = 0;
// Interrupt routine
void display() {
  noInterrupts();
  displayTree();
  displayPattern();
  interrupts();
}

void displayDebug() {
  if (debugX >= 8) {
   debugX = 0;
  }
  if (debugY >= 8) {
   debugY = 0;
  }
}

void displayTree() {
  if (tree_toggle >= 1000) {
    //displayDebug();
    if (trees_on) {
      //Serial.println("TREES OFF");
      digitalWrite(tree, LOW);
      trees_on = 0;
      /*
      int r = rows[0];
      Serial.print(cols[0]);
      Serial.println(" LOW");
      Serial.print(r);
      Serial.println(" HIGH\n");
      digitalWrite(cols[0], LOW);
      digitalWrite(r, HIGH);
      */
    } else {
      //Serial.println("TREES ON");
      digitalWrite(tree, HIGH);
      trees_on = 1;
      /*
      int r = rows[0];
      Serial.print(cols[0]);
      Serial.println(" LOW");
      Serial.print(r);
      Serial.println(" LOW\n");
      digitalWrite(cols[0], LOW);
      digitalWrite(r, LOW);
      */
    }
    tree_toggle = 0;
  }
  tree_toggle++;
}

void displayPattern() {
  digitalWrite(cols[col], HIGH);  // Turn whole previous column off
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (leds[col][7 - row] == 1) {
      digitalWrite(rows[row], HIGH);  // Turn on this led
    }
    else {
      digitalWrite(rows[row], LOW); // Turn off this led
    }
  }
  digitalWrite(cols[col], LOW); // Turn whole column on at once (for equal lighting times)
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
