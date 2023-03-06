/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library.

    Demonstrates the use of Oscil to play a wavetable.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>               // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <EventDelay.h>

#include <ezButton.h>



// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

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



note stepPiches[8] = {
    {notes[9], 250},
    {notes[6], 500},
    {notes[4], 250},
    {notes[9], 250},
    {notes[6], 100},
    {notes[4], 100},
    {notes[6], 250},
    {notes[4], 250}
};



EventDelay stepTime = EventDelay();
EventDelay noteTime = EventDelay();

unsigned int step = 0;
int stepTimeMs = 1000;

ezButton button(14); // create ezButton object that attach to pin 7;

long m = 0;

void setup()
{  
  Serial.begin(9600);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds

  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(261.63f);    // set the frequency

  stepTime.start(stepTimeMs);
  noteTime.start(1000);

  m = millis();
}

float volume = 0.12;
float gain = volume;

bool isPaused = false;


void updateControl()
{

  button.loop(); // MUST call the loop() function first

  if (button.isReleased())
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
      
      aSin.setFreq(stepPiches[step].note.freq);

      noteTime.start(stepPiches[step].noteTime);
      stepTime.start(stepTimeMs);

      Serial.printf("Step = %d - Note = %s - Freq = %f", step, stepPiches[step].note.note, stepPiches[step].note.freq);

      step = (step + 1) % 8;
    }

    if (noteTime.ready())
    {
      // note has finished playing so set volume to 0
       gain = 0;
    }
  }
}

AudioOutput_t updateAudio()
{
  return MonoOutput::from8Bit(aSin.next() * gain); // return an int signal centred around 0
}

void loop()
{
  audioHook(); // required here
}
