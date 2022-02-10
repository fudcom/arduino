/*
  Mary Had a Little Lamb Tone Generator

  circuit:
  - 8 ohm speaker on digital pin 8

  by Kris Fudalewski

  Based on:

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody
*/

#include "pitches.h"

// -- Notes in the melody / Note durations: 4 = quarter note, 8 = eighth note, etc.: rinse repeat...
int melody[] = {
  E4, 4,
  D4, 4,
  C4, 4,
  D4, 4,
  E4, 4,
  E4, 4,
  E4, 2,
  D4, 4,
  D4, 4,
  D4, 2,
  E4, 4,
  G4, 4,
  G4, 2,
  E4, 4,
  D4, 4,
  C4, 4,
  D4, 4,
  E4, 4,
  E4, 4,
  E4, 4,
  E4, 4,
  D4, 4,
  D4, 4,
  E4, 4,
  D4, 4,
  C4, 2,
  C5, 2
};

void setup() {
  // iterate over the notes of the melody:
  for (int position = 0; position < ( sizeof(melody) / sizeof(melody[0]) ); position += 2) {

    int note = melody[position];
    // to calculate the note duration, take one second divided by the note type.
    // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / melody[position + 1];

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    tone(8, note, duration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

void loop() {
  // no need to repeat the melody.
}
