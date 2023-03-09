/*  Example of Amplitude Modulation synthesis
    using Mozzi sonification library.

    Demonstrates modulating the gain of one oscillator
    by the instantaneous amplitude of another,
    shows the use of fixed-point numbers to express fractional
    values, random numbers with rand(), and EventDelay()
    for scheduling.

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
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <mozzi_fixmath.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Ead.h>

#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

// audio oscils
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);


Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE
EventDelay kDelay;           // for triggering envelope start

void setup()
{
  aModulator.setFreq(15);
  aCarrier.setFreq(440);
  startMozzi(CONTROL_RATE);
  kDelay.start(1000);
}

int env = 0;

void updateControl()
{

  if (kDelay.ready())
  {
    unsigned int attack = 300;
    unsigned int decay = 1000;
    kEnvelope.start(attack, decay);
    kDelay.start(2000);
  }

  env = (int)kEnvelope.next();
}

AudioOutput_t updateAudio()
{
  long mod = (128u + aModulator.next());
  return MonoOutput::fromNBit(24, mod * aCarrier.next() * env );
}

void loop()
{
  audioHook();
}
