#include "lcdgfx.h"
#include "lcdgfx_gui.h"

const byte RE_SWITCH = 2;
const byte RE_DATA = 4;
const byte RE_CLOCK = 7;
const long DEBOUNCE_DELAY = 25;

volatile boolean selected = false;
volatile unsigned int position = 0;
volatile byte s1 = 1, s2 = 1;
volatile long s1_debounce = 0;
volatile long s2_debounce = 0;

DisplaySSD1327_128x128_I2C display(-1); 

const char *menuItems[] =
{
    "OFF",
    "LOW",
    "MEDIUM",
    "HIGH"
};

LcdGfxMenu menu( menuItems, sizeof(menuItems) / sizeof(char *) );

void setup() {
  Serial.begin(9600);
  pinMode(RE_SWITCH, INPUT);
  pinMode(RE_DATA, INPUT);
  pinMode(RE_CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(RE_SWITCH), button, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RE_DATA), rotated_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RE_CLOCK), rotated_right, CHANGE);
  
  display.begin();
  display.setFixedFont(ssd1306xled_font8x16);
  display.clear();
  display.setColor(GRAY_COLOR4(255));
  menu.show( display );

}

void loop() {

//  byte dt = digitalRead(RE_DATA);
//  byte clk = digitalRead(RE_CLOCK);
//  byte sw = digitalRead(RE_SWITCH);
//
//  Serial.print(dt);
//  Serial.print(" ");
//  Serial.print(clk);
//  Serial.print(" ");
//  Serial.print(sw);
//  Serial.print(" ");
//  Serial.print(position);
//  Serial.print(" ");
//  Serial.println(selected);
//  delay(100);

  int menu_position = position % menu.size();

  if ( menu_position != menu.selection() ) {
    Serial.print("P: ");
    Serial.println(menu_position);
    while ( menu_position < menu.selection() ) {
      menu.up();
    }
    while ( menu_position > menu.selection() ) {
      menu.down();
    }
    menu.show( display );
  }
  
}

void button() {
  selected = ( digitalRead(RE_SWITCH) == 0 );
}


void rotated_left() {
  s1 = digitalRead(RE_DATA);
  if (s1 < s2 && ( s1_debounce + DEBOUNCE_DELAY < millis() ) ) {
    s1_debounce = millis();
    position--;
  }
}

void rotated_right() {
  s2 = digitalRead(RE_CLOCK);
  if (s2 < s1 && ( s2_debounce + DEBOUNCE_DELAY < millis() ) ) {
    s2_debounce = millis();
    position++;
  }
}
