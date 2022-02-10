/*
 * Scroller on a 4 digit, 7 segment, clock radio display.
 *
 * by Kris Fudalewski
 * 
 * www.fudcom.com
 * 
 * WARNING: This sketch is designed for a very specific display, anything else might damage your board!  
 *          It is also assumed that the proper current resistors (150 ohm per anode, 100 ohm per cathode) 
 *          are installed to prevent over current to the display.
 */

// -- Arduino pin to display.
const byte BANK_PINS[] = { 12, 11 };
const byte SEGMENT_PINS[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 13, A0, A1, A2, A3, A4, A5 };

// -- { BANK, SEGMENT } mapping for each digit in display.
const byte NUMBER_ARRAY[][7][2] = { 
  { { 0,  3 }, { 99, 99 }, { 1,  2 }, { 1,  3 }, { 1,  4 }, { 1,  5 }, { 0,  4 } },   // -- 99 = There's a segment missing here, don't try to render it. :'(
  { { 1,  8 }, {  0,  8 }, { 1,  6 }, { 0,  6 }, { 0,  5 }, { 1,  7 }, { 0,  7 } },
  { { 0,  9 }, {  1,  9 }, { 0, 10 }, { 1, 10 }, { 1, 12 }, { 0, 11 }, { 1, 11 } },
  { { 1, 15 }, {  0, 15 }, { 1, 13 }, { 0, 13 }, { 0, 12 }, { 1, 14 }, { 0, 14 } }
};

/* A simple font, each letter is 7 bits corresponding to the segments in a digit:
 * 
 *    0
 *  1   2 
 *    3
 *  4   5
 *    6
 */
const byte FONT[] = {
  0b0000000, // { }                       // -- [Space]
  0,0,0,0,0,0,0,0,0,0,0,0,                // -- Unsupported
  0b0001000, // { 3 }                     // -- [Dash]
  0,0,                                    // -- Unsupported
  0b1110111, // { 0, 1, 2, 4, 5 },        // -- 0
  0b0010010, // { 2, 5 },                 // -- 1
  0b1011101, // { 0, 2, 3, 4, 6 },        // -- 2
  0b1011011, // { 0, 2, 3, 5, 6 },        // -- 3
  0b0111010, // { 1, 2, 3, 5 },           // -- 4
  0b1101011, // { 0, 1, 3, 5, 6 },        // -- 5
  0b1101111, // { 0, 1, 3, 4, 5, 6 },     // -- 6
  0b1010010, // { 0, 2, 5 },              // -- 7
  0b1111111, // { 0, 1, 2, 3, 4, 5, 6 },  // -- 8
  0b1111011, // { 0, 1, 2, 3, 5, 6 },     // -- 9
  0,0,0,                                  // -- Unsupported
  0b1111110, // { 3, 6 },                 // -- [Equals]
  0,0,0,                                  // -- Unsupported
  0b1111110, // { 0, 1, 2, 3, 4, 5 },     // -- A
  0b0101111, // { 1, 3, 4, 5, 6 },        // -- B
  0b1100101, // { 0, 1, 4, 6 },           // -- C
  0b0011111, // { 2, 3, 4, 5, 6 },        // -- D
  0b1101101, // { 0, 1, 3, 4, 6 },        // -- E
  0b1101100, // { 0, 1, 3, 4 },           // -- F
  0b1101111, // { 0, 1, 3, 4, 5, 6 },     // -- G
  0b0111110, // { 1, 2, 3, 4, 5 },        // -- H
  0b0010010, // { 2, 5 },                 // -- I
  0b0010011, // { 2, 5, 6 },              // -- J
  0b0111110, // { 1, 2, 3, 4, 5 },        // -- K
  0b0100101, // { 1, 4, 6 },              // -- L
  0b0001110, // { 3, 4, 5 },              // -- M   TODO: Post process over two characters.
  0b0001110, // { 3, 4, 5 },              // -- N
  0b0001111, // { 3, 4, 5, 6 },           // -- O
  0b1111100, // { 0, 1, 2, 3, 4 },        // -- P
  0b1111010, // { 0, 1, 2, 3, 5 },        // -- Q
  0b0001100, // { 3, 4 },                 // -- R
  0b1101011, // { 0, 1, 3, 5, 6 },        // -- S
  0b0011010, // { 2, 3, 5 },              // -- T
  0b0110111, // { 1, 2, 4, 5, 6 },        // -- U
  0b0000111, // { 4, 5, 6 },              // -- V
  0b0000111, // { 4, 5, 6 },              // -- W   TODO: Post process over two characters.
  0b0111110, // { 1, 2, 3, 4, 5 },        // -- X
  0b0111010, // { 1, 2, 3, 5 },           // -- Y
  0b1011101, // { 0, 2, 3, 4, 6 },        // -- Z
  0,0,0,0,                                // -- Unsupported
  0b0000001  // { 6 },                    // -- [Underscore]
};

const String TEXT         = "HELLO THIS IS A TEST    FUDALEWSKI RULES     GREETS TO  ANDY  DANNY  FRANKY  LILLY-ANNA  ROSELINA  MARY-LOU         ";
const int    SCROLL_SPEED = 15;

void setup() {

  /* Initialize bank pins, set them BOTH to HIGH. These are actually current sinks. 
   *  
   * This may sound counter intuitive, however we take advantage of the LED display here,
   * as current will not travel in reverse.  
   */ 
  for (byte number = 0; number < sizeof(BANK_PINS); number++) {
    pinMode(BANK_PINS[number], OUTPUT);
    digitalWrite(BANK_PINS[number], HIGH);
  }
  
  /* Initialize segment pins to LOW to turn them all off, again, since these are LEDS they shouldn't allow current in reverse. */
  for (byte number = 0; number < sizeof(SEGMENT_PINS); number++) {
    pinMode(SEGMENT_PINS[number], OUTPUT);
    digitalWrite(SEGMENT_PINS[number], LOW);
  }

}

void loop() {

  render(TEXT, scroll());  
  
}

int scroll_offset = 0;
int scroll_interval = 0;

int scroll() {

  scroll_interval++;

  if ( scroll_interval > SCROLL_SPEED ) {
    scroll_offset++;
    scroll_interval = 0;
  }

  return scroll_offset;
  
}

const int SCREEN_SIZE = 4;
const int BANK_SWITCH_DELAY = 10;

/*
 * Renders the given text on the display. A starting offset in the string can be specified to scroll through the text. 
 * 
 * Multi-cathode displays require a bank switch in order to reach all of the LEDs in the display. It allows us to safely 
 * drive the display from an Arduino as only half the LEDs are active at a time.
 * 
 * In my tests, the current peaks at 25ma, when all LEDs for a give bank are active when using 150 ohm resistors 
 * on the anode and 100 ohm on the cathode. This setup also provides a voltage divider from the arduino's 5V down
 * to 2V to really under drive the LEDs.
 */
void render(String text, int offset) {
  
  for (byte bank = 0; bank < sizeof(BANK_PINS); bank++) {
    // -- Enable desired segements for bank.
    for (int c = 0; c < SCREEN_SIZE; c++) {
      int rolled_offset = ( c + offset ) % text.length();
      int font_index = text[rolled_offset] - 32;
      int font_char = 0;   // -- Blank
      if (font_index >= 0 && font_index < sizeof(FONT)) {
        font_char = FONT[font_index];
      }
      for (byte b = 64, s = 0; b > 0; b >>= 1, s++) {
        if ( NUMBER_ARRAY[c][s][0] == bank ) {
          if ( ( font_char & b ) == b ) {
              digitalWrite(SEGMENT_PINS[NUMBER_ARRAY[c][s][1]], HIGH);    
          } else {
              digitalWrite(SEGMENT_PINS[NUMBER_ARRAY[c][s][1]], LOW);    
          }
        } 
      }
    } 
    digitalWrite(BANK_PINS[bank], LOW);   // -- Enable bank.
    delay(BANK_SWITCH_DELAY);             // -- Wait a sec.
    digitalWrite(BANK_PINS[bank], HIGH);  // -- Disable bank.
    
  }
}
