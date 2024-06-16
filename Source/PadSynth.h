/*
  ==============================================================================

    PadSynth.h
    Created: 1 Mar 2024 8:02:46pm
    Author:  70

  ==============================================================================
*/
#pragma once

#include "Oscillators.h"
#include <JuceHeader.h>

/**
    Represents a pad synthesizer with phase modulation and a low-pass filter.

    PadSynth employs a sine oscillator modulated by an LFO for phase modulation effects, combined with a low-pass filter to sculpt the tonal characteristics. Initialize with setSampleRate(), then set the oscillator and filter parameters to craft atmospheric textures. The process() function generates the audio output, blending modulated and filtered signals for rich, dynamic pads suitable for ambient and electronic music contexts.
*/
class PadSynth
{
public:
    PadSynth()
    {
        updateFilter(filterCutOff); // Initialize the filter with the default cutoff frequency
    }
    
    // Sets the sample rate for the oscillators and updates the filter
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        sinOsc.setSampleRate(sampleRate);
        sinLFO.setSampleRate(sampleRate);
        updateFilter(filterCutOff);
    }

    // Sets the cutoff frequency of the low-pass filter. 
    void setFilterCutOff(float CutOffFreq)
    {
        filterCutOff = CutOffFreq;
        updateFilter(filterCutOff);
    }
    
    // Sets the frequency of the sine oscillator
    void setFrequency(float Freq)
    {
        Frequency = Freq;
        sinOsc.setFrequency(Frequency);
    }

    // Sets the frequency of the LFO
    void setLFOFrequency(float LFOFreq)
    {
        LFOFrequency = LFOFreq;
        sinLFO.setFrequency(LFOFrequency);
    }
    
    // Sets the amount of LFO modulation
    void setLFOAmount(float LFOAmt) // Amount range 0~1
    {
        LFOAmount = LFOAmt;
    }
    
    // Processes the audio signal, applying LFO modulation and filtering
    float process()
    {
        // Calculate the LFO effect for phase modulation
        auto LFOWave = sinLFO.process() * LFOAmount; // LFO output modulates around 0

        // Modulate the sine oscillator's phase
        auto sinPhase = sinOsc.getPhase();
        float modulatedPhase = LFOWave + sinPhase * 2.0 * M_PI;
        float modSinWave = std::sin(modulatedPhase);

        // Mix the raw and filtered waveforms
        return modSinWave * 0.2 + lowPassFilter.processSingleSampleRaw(modSinWave) * 0.8;
    }
private:
    SinOsc sinOsc;
    SinOsc sinLFO;
    juce::IIRFilter lowPassFilter;
    
    float sampleRate = 44100.0f;
    float Frequency = 440.0f;
    float LFOFrequency = 5.0f;    // Default LFO frequency
    float LFOAmount = 0.5f;       // Default LFO modulation amount
    float filterCutOff = 5000.0f; // Default filter cutoff frequency

    // Updates the low-pass filter's coefficients based on the current cutoff frequency
    void updateFilter(float cutoffFrequency)
    {
        lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffFrequency, 1.0));
        lowPassFilter.reset();
    }
};

