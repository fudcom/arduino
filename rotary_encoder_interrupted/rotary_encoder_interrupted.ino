/*
 * Interrupted Rotary Encoder Demo
 *
 * by Kris Fudalewski
 * 
 * www.fudcom.com
 */

const byte RE_SWITCH = 2;
const byte RE_DATA = 4;
const byte RE_CLOCK = 7;
const long DEBOUNCE_DELAY = 25;

volatile boolean selected = false;
volatile unsigned int position = 0;
volatile byte s1 = 1, s2 = 1;
volatile long s1_debounce = 0;
volatile long s2_debounce = 0;

void setup() {

  Serial.begin(9600);

  pinMode(RE_SWITCH, INPUT);
  pinMode(RE_DATA, INPUT);
  pinMode(RE_CLOCK, INPUT);

  attachInterrupt(digitalPinToInterrupt(RE_SWITCH), button, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RE_DATA), rotated_left, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RE_CLOCK), rotated_right, CHANGE);
  
}

void loop() {

  Serial.print(position);
  Serial.print(" ");
  Serial.println(selected);
  delay(100);
 
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
