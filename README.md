# WanderingInCycle

## Overview
This drone composition revolves around a fixed motif and consistent chord progression, providing a
stable foundation. While grounded in this structural core, the piece exhibits dynamic variations
through the incorporation of random pitch bounces and embellishments, introducing a rich variety of
intervals and rhythmic changes. Timbral exploration is a central aspect, achieved through the realtime
manipulation of multiple Low Frequency Oscillators (LFOs) as input parameters. This technique
allows for constantly evolving subtle differences in the sound, avoiding repetitive auditory experiences.
Furthermore, the application of reverb across the entire composition softens the overall timbre,
contributing to a cohesive and immersive sonic experience.

## Detailed Description of Timbral Elements
The composition is primarily composed of three distinct timbres: subbass, pad, and string.

### A. Subbass
The subbass is a combination of saw and square waves, occupying the low-frequency range with a
constant pitch. Subtle timbral variations are achieved by dynamically controlling the pulse width,
square wave amount, and fine detune, adding rich harmonics to the low frequencies.

### B. Pad
The pad sound is created with two sine oscillators and undergoes Phase Modulation, producing a rich
and evolving timbre.

1. Chord Progression: This timbre forms the backbone of the piece with chords played in the
sequence of Fmaj7 - Dm7 - Am7 - Em7.

2. Bounce: To create an auditory difference between the left and right ears, the pitch change
speed in the right ear is twice as fast as in the left. In addition, a movement class is used to
generate the sensation of bounce. Initially, the movement value was derived from multiplying
a square wave by a sine wave. However, this method led to an issue: the square wave's sharp
transitions caused unpleasant clicking sounds. To solve this problem, the initial strategy was
revised by removing the square wave oscillator and instead employing sine wave distortion.
This adjustment successfully prevents sudden volume jumps while preserving the sound's
granularity, ensuring a smoother amplitude change.

3. Embellishment: This part supplements the high frequencies and features dynamic panning,
creating a back-and-forth movement from the left ear to the right.

### C. String
The string timbre is a combination of saw and square waves, incorporating Frequency Modulation
with a sine LFO controlling the frequency to simulate the vibrato of string instruments.

1. Root Note: This part emphasizes the root note from the pad chords, adding rich harmonics
and using an LFO to create a slow fade in and out.

2. Motif: The main melody and its octave-higher counterpart form the motif, with an LFO
adjusting the saw wave amount of the higher octave to introduce subtle timbral variations.

## Install instruction
For Mac, just use projucer to open the source code and build the plugin.

Standalone version is the most convenient way to play this drone music.

If you have some questions, feel free to contact me through email: showyeah70@gmail.com

## Bibliography
Ableton. (2024). Changing the sound. Learning Synths. https://learningsynths.ableton.com/en/synthbasics/
changing-the-sound

Ableton. (2024). Strings. Learning Synths. https://learningsynths.ableton.com/en/recipes/strings

Ableton. (2024). Subbass. Learning Synths. https://learningsynths.ableton.com/en/recipes/subbass

ELEH. (2019). Collect Yourself - Well-arranged. On Living Space. Retrieved from
https://elehreleases.bandcamp.com/track/collect-yourself-well-arranged

Write A Song. (2023, May 19). Top 21 Chord Progressions That Use Minor 9th. YouTubeVideo, 6:34.
https://www.youtube.com/watch?v=-pICMA__FJ0
