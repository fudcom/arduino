/*
* Interrupted Ultrasonic Sensor for Arduino UNO Sketch
*
* by Kris Fudalewski
* 
* www.fudcom.com
*
*/

// defines pins numbers
const int trigPin = 12;
const int echoPin = 13;

// other constants
const int SONAR_PING_DURATION = 12;
const int SONAR_ABORT_DURATION = 5000;

// defines variables
int sonarState = -1;
int sonarListenState = -2;

volatile int distance;

void setup() {

  setupSonarMonitor();

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  Serial.begin(2000000); // Starts the serial communication
  
}

void loop() {

  Serial.println( distance );

  delay(100);

}

// -- Interrupt entry point.
ISR(TIMER2_COMPA_vect){

  // -- Ping sonar senor.
  if ( sonarState <= SONAR_PING_DURATION ) {
    // -- Reset sonar sensor
    if ( sonarState == 0 ) {
      digitalWrite(trigPin, LOW);
    // -- Start ping
    } else if ( sonarState == 2 ) {
      digitalWrite(trigPin, HIGH);
    // -- End ping
    } else if ( sonarState == SONAR_PING_DURATION ) {
      digitalWrite(trigPin, LOW);
    } 
  // -- Listen for up to SONAR_ABORT_DURATION.
  } else if ( sonarState < SONAR_ABORT_DURATION ) {
    int sonarSignal = digitalRead(echoPin);
    // -- Wait for radio silence
    if ( sonarSignal == LOW && sonarListenState == -2 ) {
      sonarListenState = -1;
    // -- Start listening! 
    } else if ( sonarSignal == HIGH && sonarListenState == -1 ) {
      sonarListenState = sonarState;
    // -- Done listening, determine distance.
    } else if ( sonarSignal == LOW && sonarListenState > 0 ) {
      distance = sonarState - sonarListenState;
      sonarState = -1;
      sonarListenState = -2;
    }
  } else {
    distance = -1;
    sonarState = -1;
    sonarListenState = -2;
  }
  
  sonarState++;

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
