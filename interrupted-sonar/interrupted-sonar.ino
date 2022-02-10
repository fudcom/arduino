/*
 * Interrupted Ultrasonic Sensor for Arduino UNO Sketch
 *
 * by Kris Fudalewski
 * 
 * www.fudcom.com
 */

// -- Pin constants
const int SONAR_TRIGGER_PIN = 12;
const int SONAR_ECHO_PIN = 13;

// -- Sonar monitor state constants and variables
const int SONAR_STATE_RESET = -1;
const int SONAR_STATE_REST_DURATION = 2;
const int SONAR_STATE_PING_DURATION = 12;
const int SONAR_STATE_ABORT_DURATION = 5000;
const int SONAR_LISTEN_STATE_RESET = -2;
const int SONAR_LISTEN_STATE_LISTENING = -1;
const int SONAR_LISTEN_STATE_ENDED = 0;

int sonarState = SONAR_STATE_RESET;                 
int sonarListenState = SONAR_LISTEN_STATE_RESET;

// -- Distance constants and variables
const int DISTANCE_UNKNOWN = -1;

volatile int distance;

void setup() {

  setupSonarMonitor();

  pinMode(SONAR_TRIGGER_PIN, OUTPUT); 
  pinMode(SONAR_ECHO_PIN, INPUT);     
  
  Serial.begin(9600);      
  
}

void loop() {

  // -- Just display the contents of the 'distance' variable. This gets updated by the timer interrupt.
  Serial.println( distance );

  delay(100);

}

// ---------------------------------------------
// ------ Timer interrupt bootstrap code -------
// ---------------------------------------------

// -- Setup a 1 MHZ timer interrupt to monitor sonar sensor.
void setupSonarMonitor() {
  
  cli();                    // disable interrupts

  TCCR2A = 0;               // Reset TIMER2 registers
  TCCR2B = 0;               
  TCNT2  = 0;               
  
  TCCR2A = (1 << WGM21);   // turn on CTC mode
  TCCR2B = (1 << CS21);    // Set 8x prescaler
  OCR2A  = 1;              // set 1 mhz interval on 8x prescaler -> (16 mhz) / (8 * 1 mhz) - 1 
  TIMSK2 |= (1 << OCIE2A); // enable timer compare interrupt

  sei();                   // enable interrupts again
  
}

// -- Interrupt entry point. 
// -- Note: There are no loops or function calls in the interrupt to keep it fast. 
ISR(TIMER2_COMPA_vect){

  // -- Sonar monitor PING state.
  if ( sonarState <= SONAR_STATE_PING_DURATION ) {
    // -- Reset sonar sensor
    if ( sonarState == 0 ) {
      digitalWrite(SONAR_TRIGGER_PIN, LOW);
    // -- Start ping after a moment of rest
    } else if ( sonarState == SONAR_STATE_REST_DURATION ) {
      digitalWrite(SONAR_TRIGGER_PIN, HIGH);
    // -- End ping
    } else if ( sonarState == SONAR_STATE_PING_DURATION ) {
      digitalWrite(SONAR_TRIGGER_PIN, LOW);
    } 
  // -- Sonar monitor LISTEN state.
  } else if ( sonarState < SONAR_STATE_ABORT_DURATION ) {
    int sonarSignal = digitalRead(SONAR_ECHO_PIN);
    // -- Sonar sensor might be HIGH just after generating a pulse, wait until it calms down.
    if ( sonarSignal == LOW && sonarListenState == SONAR_LISTEN_STATE_RESET ) {
      sonarListenState = SONAR_LISTEN_STATE_LISTENING;
    // -- Listen until a signal is detected.
    } else if ( sonarSignal == HIGH && sonarListenState == SONAR_LISTEN_STATE_LISTENING ) {
      sonarListenState = sonarState;
    // -- Done listening, determine distance.
    } else if ( sonarSignal == LOW && sonarListenState > SONAR_LISTEN_STATE_ENDED ) {
      distance = sonarState - sonarListenState;
      sonarState = SONAR_STATE_RESET;
      sonarListenState = SONAR_LISTEN_STATE_RESET;
    }
  // -- Sonar monitor ABORT state.
  } else {
    distance = DISTANCE_UNKNOWN;
    sonarState = SONAR_STATE_RESET;
    sonarListenState = SONAR_LISTEN_STATE_RESET;
  }

  // -- Increment sonar state each cycle.
  sonarState++;

}
