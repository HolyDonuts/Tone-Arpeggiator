# Tone-Sequencer
A tone based sequencer using only a single Rotary Encoder and Piezo Buzzer. Allows changing the frequency of each step and tempo

The tone sequencer plays through an 8 step array of frequencies at a set interval between each item in the array, effectively acting as a sequencer.

A single rotary encoder is connected to pins 3,4,5 (CLK, DT, SW) 
A piezo buzzer is connected to pin 6

For demo purposes a little melody is programmed at 120bpm, or 250ms between each step on start up.

There are 3 modes available based on the button press count. Each of these affect what the encoder knob canges - Step Select, Edit Step, Edit Tempo
The frequencies available to select while in edit mode are based on the A Minor scale and range from A3 (220hz) to A5 (880hz)

- By default the encoder is in <strong>Step Select mode</strong>. Choose which step you want to edit the frequency of from 0-7.
- Click the button once to enter <strong>Edit Step mode</strong>. Rotating the encoder while in Edit Step will change the freqency of the selected step.
- Click the button a second time to enter <strong>Edit Tempo mode</strong>. Rotating the encoder while in Edit Tempo mode will change the interval between each step (in other words the bpm of the whole sequence).
- Click the button a third time to go back to <strong>Step Select mode</strong>



Potential to do's:
1. Add RC filter to circuit to try get a smoother transition on knob turn. Currently it's a bit finicky and you have to be real slow and careful when turning the knob, or else the response can jump around a bit.
2. Add step check so when editing the frequency of a step it starts at the current step's frequency. Currently when you turn the knob it starts at the last selected frequency for the last edited step
3. Add more buttons to make it quicker to update 
4. Maybe allow the encoder to cycle through steps when it hits it's min/max (e.g., when in Step Select Mode, when increasing and max step - 7 - is reached cycle back round to min step - 0)
5. Figure out how to change the length of each step independently, so they can all be different lengths, creating more interesting melodies.
6. Start figuring out how to convert this to Mozzi library to be able to add affects and filters to the output (or better yet, every step!!)
