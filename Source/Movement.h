/*
  ==============================================================================

    Movement.h
    Created: 2 Mar 2024 2:18:52pm
    Author:  70

  ==============================================================================
*/
#pragma once

#include "Oscillators.h"
#include <JuceHeader.h>

/**
    Represents an amplitude control mechanism with vibrato and waveform mixing capabilities.

    Movement is an amplitude control module. Configure it with setSampleRate(), setFrequency(), and adjust vibrato and waveform mixing settings for diverse results. The process() method synthesizes the final audio output, blending square and sine waves modulated by vibrato for complex volume changes.
*/
class Movement
{
public:
    
    // Sets the sample rate for the internal oscillators and LFO
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        sinOsc.setSampleRate(sampleRate);
        vibratoLFO.setSampleRate(sampleRate);
    }

    // Sets the base frequency for the oscillators
    void setFrequency(float Freq)
    {
        baseFrequency = Freq;
        sinOsc.setFrequency(Freq);
    }

    // Sets the frequency of the vibrato effect
    void setVibratoFreq(float VF) // sin LFO
    {
        VibratoFreq = VF;
        vibratoLFO.setFrequency(VibratoFreq);
    }
    
    // Sets the amount of vibrato effect to apply
    void setVibratoAmount(float VibratoAmt) // Amount range 0~1
    {
        VibratoAmount = VibratoAmt;
    }
    
    // Processes the audio signal, applying vibrato and mixing waveforms
    float process()
    {
        // Calculate the vibrato effect
        auto vibratoEffect = vibratoLFO.process() * VibratoAmount; // LFO output modulates around 0
        auto modulatedFrequency = baseFrequency * (1.0f + vibratoEffect);

        // Apply the modulated frequency to the oscillators
        sinOsc.setFrequency(modulatedFrequency);
        
        // Generate the waveforms and scale it to 0~0.5
        auto sinVal = (sinOsc.process() + 1) / 2;
        
        // Apply a simple distortion effect by clipping the sine wave at 0.5
        if (sinVal > 0.5)
            sinVal = 0.5f;

        return sinVal;

    }
private:
    SinOsc sinOsc;
    SinOsc vibratoLFO;
    
    float sampleRate = 44100.0f;
    float baseFrequency = 440.0f;
    float VibratoFreq = 5.0f;     // Default vibrato frequency
    float VibratoAmount = 0.005f; // Default vibrato amount
};


