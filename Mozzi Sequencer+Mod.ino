

#include <MozziGuts.h>
#include <Oscil.h>               // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <EventDelay.h>
#include <tables/envelop2048_uint8.h>
#include <ezButton.h>

// Rotary encoder
const int encoderPinA = 2;
const int encoderPinB = 3;

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aModulator(SIN2048_DATA);
Oscil <ENVELOP2048_NUM_CELLS, AUDIO_RATE> aEnvelop(ENVELOP2048_DATA);


// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 640 // Hz, powers of 2 are most reliable

typedef struct
{
  char *note;
  float freq;
} noteFreq;

typedef struct
{
  noteFreq note;
  float noteTime;
} note;

//Stores the frequency in a value from 0-14 which is then used to compare noteFreq and editStep to reset the knob counter to the current edit step's frequency
int freqState[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

noteFreq notes[] = {
    {"A3", 220},
    {"B3", 249.94},
    {"C4", 261.94},
    {"D4", 293.66},
    {"E4", 329.63},
    {"F4", 349.23},
    {"G4", 392},
    {"A4", 440},
    {"B4", 493.88},
    {"C5", 523.25},
    {"D5", 587.33},
    {"E5", 659.25},
    {"F5", 698.25},
    {"G5", 783.99},
    {"A5", 880}
    // TODO add the rest of the notes and frequences
};



note stepPitches[8] = {
    {notes[5], 500},
    {notes[7], 250},
    {notes[6], 750},
    {notes[5], 125},
    {notes[3], 500},
    {notes[2], 125},
    {notes[3], 750},
    {notes[4], 1000}
};



EventDelay stepTime = EventDelay();
EventDelay noteTime = EventDelay();

int currentStateCLK;  //check the current state of the knob button
int lastStateCLK; 
unsigned int step = 0;
int stepTimeMs = 1000;
unsigned int editStep = 0;


ezButton pauseButton(12); // create ezButton object that attach to pin 7;
ezButton upButton(10);
ezButton downButton(5);
ezButton actionButton(11);
int btnState;
int modFreq;


long m = 0;
int stepFreq = 0;

void setup()
{  
  Serial.begin(9600);
  pauseButton.setDebounceTime(50); // set debounce time to 50 milliseconds
  upButton.setDebounceTime(100);
  downButton.setDebounceTime(100);
  actionButton.setDebounceTime(100);
  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(261.63f);    // set the frequency
  Serial.begin(9600);
  stepTime.start(stepTimeMs);
  noteTime.start(1000);


  m = millis();

  // Setup encoder pins
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
    //Setup last state of encoder
  lastStateCLK = digitalRead(encoderPinA);

}

float volume = 0.12;
float gain = volume;

bool isPaused = false;


void updateControl()
{
  knob();
  checkButtons();
}

AudioOutput_t updateAudio()
{
  long mod2 = (128u+ aModulator.next());
  return MonoOutput::fromNBit(16, mod2 * aSin.next());
}

void loop()
{
  audioHook(); // required here
}

void knob() {
  currentStateCLK = digitalRead(encoderPinA);
  if (btnState == 1){  
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(encoderPinB) != currentStateCLK) {
      stepFreq = stepFreq - 1;
    } else {
      stepFreq = stepFreq + 1;
    }
    if (stepFreq > 14) {
      stepFreq = 0;
    } else if (stepFreq < 0) {
      stepFreq = 14;
    }
    //freq = freqState[editStep];
    stepPitches[editStep].note.freq = notes[stepFreq].freq;
  }

  lastStateCLK = currentStateCLK;
  
  } else if (btnState == 0){
    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
    if (digitalRead(encoderPinB) != currentStateCLK) {
      modFreq = modFreq - 1;
    } else {
      modFreq = modFreq + 1;
    }
    aModulator.setFreq(modFreq);
    //stepPitches[editStep].noteTime = modFreq;
    Serial.println(modFreq);
  }
modFreq = constrain(modFreq, 0, 1000);
  lastStateCLK = currentStateCLK;
  }

 

}


void checkButtons(){
  
  pauseButton.loop(); // MUST call the loop() function first
  upButton.loop();
  downButton.loop();
  actionButton.loop();
  actionButton.getState();

  if (pauseButton.isReleased())
  {
    isPaused = !isPaused;
    gain = isPaused ? 0 : volume;
  }
  
  if (!isPaused)
  {
    if (stepTime.ready())
    {
      m = millis();
      Serial.println(m);
      gain = volume;
      
      aSin.setFreq(stepPitches[step].note.freq);

      noteTime.start(stepPitches[step].noteTime);
      stepTime.start(stepTimeMs);

      step = (step + 1) % 8;
    }

    if (noteTime.ready())
    {
      // note has finished playing so set volume to 0
       gain = 0;
    }
  }
  if (upButton.isPressed())
    editStep = editStep + 1;
  if (editStep > 7) {
    editStep = 0;
  }

  if (downButton.isPressed())
    editStep = editStep - 1;
  if (editStep < 0) {
    editStep = 7;
  }


btnState = actionButton.getState();


}

