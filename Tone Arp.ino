// Rotary encoder pins
const int encoderPinA = 3;
const int encoderPinB = 4;
const int encoderButton = 5;
// Piezo buzzer pin
const int buzzerPin = 6;

// Pitch of each step (in Hz)
//int pitch[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
int pitch[] = { 220, 261.63, 329.63, 523.25, 329.63, 587.33, 880 , 440 };

//Frequencies (in Hz) of A Minor Scale
float freqArray[] = { 220, 249.94, 261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.25, 698.46, 783.99, 880 };
//Equivalent Note      A3    B3      C4     D4       E4      F4     G4   A4    B4      C5      D5      E5      F5      G5     A5

//Tempo in Millis.
long intervalArray[] = {375, 300, 250, 214, 188};
//Equivalent BPM         80  100  120  140  160

// Rotary encoder variables
int currentStateCLK;  //check the current state of the knob button
int lastStateCLK;     // store the last state of the knob button
int buttonCount = 0;
unsigned long lastButtonPress = 0;  // store the previous count of the button press
unsigned long lastTurn = 0;
int step = 0;
int lastStep = 0;
int editStep = 0;
int editLastStep = 0;
int freq = 0;
int intervalCount = intervalArray[2];

// Timekeeping variables
unsigned long currentTime = 0;
unsigned long previousTime = 0;
long interval = intervalCount;  // Time interval between steps in milliseconds. Starts at 120bpm, or 250ms between each step.

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  // Setup encoder pins
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(encoderButton, INPUT_PULLUP);
  // Setup piezo buzzer pin
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  //Check button state and count
  int btnState = digitalRead(encoderButton);
  //If we detect LOW signal, button is pressed
  if (btnState == LOW && buttonCount == 0) {
    //if 1000ms have passed since last LOW pulse, it means that the button has been pressed, released and pressed again
    if (millis() - lastButtonPress > 1000) {
      buttonCount = 1;
      Serial.println("Set Frequency");
    }
    lastButtonPress = millis();
  } else if (btnState == LOW && buttonCount == 1) {
    if (millis() - lastButtonPress > 1000) {
      buttonCount = 2;
      Serial.println("Select Tempo");
    }
    lastButtonPress = millis();
  } else if (btnState == LOW && buttonCount == 2) {
    if (millis() - lastButtonPress > 1000) {
      buttonCount = 0;
      Serial.println("Select Step");
    }
    lastButtonPress = millis();
  }


  //Rotary Encoder Knob functions based on buttonCount
  if (buttonCount == 0) {
    currentStateCLK = digitalRead(encoderPinA);
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
      if (millis() - lastTurn > 1000) {
        if (digitalRead(encoderPinB) != currentStateCLK) {
          editStep = editStep - 1;
        } else {
          editStep = editStep + 1;
        }
        lastTurn = millis();
      }
      editStep = constrain(editStep, 0, 7);
    }
    lastStateCLK = currentStateCLK;
  } else if (buttonCount == 1) {
    currentStateCLK = digitalRead(encoderPinA);
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
      if (digitalRead(encoderPinB) != currentStateCLK) {
        freq = freq - 1;
      } else {
        freq = freq + 1;
      }
      freq = constrain(freq, 0, 14);
      pitch[editStep] = freqArray[freq];
    }
    lastStateCLK = currentStateCLK;
  } 
  else if (buttonCount == 2) {
    currentStateCLK = digitalRead(encoderPinA);
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
      if (digitalRead(encoderPinB) != currentStateCLK) {
        intervalCount = intervalCount - 1;
      } else {
        intervalCount = intervalCount + 1;
      }
      intervalCount = constrain(intervalCount, 0, 4);
      interval = intervalArray[intervalCount];
    }
    lastStateCLK = currentStateCLK;
  }

//Play the current step freq
  tone(buzzerPin, pitch[step]);

  // Check if it's time to move to the next step
  currentTime = millis();
  if (currentTime - previousTime >= interval) {
    // Move to the next step
    step = (step + 1) % 8;
    previousTime = currentTime;
  }

//Printing functions
  Serial.print("Tempo(ms): ");
  Serial.print(interval);
  Serial.print("  -  ");
  Serial.print("Edit Step: ");
  Serial.print(editStep);
  Serial.print("  -  ");
  Serial.print("Step Freqs: ");
  for (int x = 0; x <= 7; x++) {
    Serial.print(pitch[x]);
    Serial.print(" - ");
  }
  Serial.println(" ");
}
