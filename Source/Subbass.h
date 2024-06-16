/*
  ==============================================================================

    Subbass.h
    Created: 2 Mar 2024 2:52:28pm
    Author:  70

  ==============================================================================
*/
#pragma once

#include "Oscillators.h"
#include <JuceHeader.h>
#include <cmath>

/**
    Generates deep subbass sounds with versatile modulation and filtering options.

    Subbass synthesizes low-frequency audio using square and saw oscillators, enriched with vibrato and detune effects, and shaped by a low-pass filter for smooth textures. Configure it with setSampleRate() and setFrequency(), then modulate with vibrato, detune, and filter settings to craft rich basslines. The process() method outputs the final mixed and filtered audio signal, suitable for electronic music production.
*/
class Subbass
{
public:
    Subbass()
    {
        updateFilter(filterCutOff); // Initialize the filter with the default cutoff frequency
    }
    
    // Sets the sample rate for the sub-bass module and its components
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        squareOsc.setSampleRate(sampleRate);
        sawOsc.setSampleRate(sampleRate);
        vibratoLFO.setSampleRate(sampleRate);
        detuneFine.setSampleRate(sampleRate);
        detuneCoarse.setSampleRate(sampleRate);
    }

    // Sets the base frequency for the oscillators
    void setFrequency(float Freq)
    {
        baseFrequency = Freq;
        squareOsc.setFrequency(baseFrequency);
        sawOsc.setFrequency(baseFrequency);
    }
    
    // Sets the frequency of the vibrato effect
    void setVibratoFreq(float VF)
    {
        VibratoFreq = VF;
        vibratoLFO.setFrequency(VibratoFreq);
    }
    
    // Sets the fine detune amount in cents
    void setDetuneFine(int cents)
    {
        DetuneFine = cents;
        float DFfreq = baseFrequency * pow(2.0, DetuneFine / 1200.0);
        detuneFine.setFrequency(DFfreq);
    }
    
    // Sets the coarse detune amount in semitones
    void setDetuneCoarse(int semitones)
    {
        DetuneCoarse = semitones;
        float DCfreq = baseFrequency * pow(2.0, DetuneCoarse / 12.0);
        detuneCoarse.setFrequency(DCfreq);
    }
    
   // Sets the pulse width of the square oscillator
    void setSquarePulseWidth(float PW)
    {
        squareOsc.setPulseWidth(PW);
    }

    // Sets the mix amount for the square wave oscillator
    void setSquareAmount(float SquareAmt)
    {
        SquareAmount = SquareAmt;
    }
    
    // Sets the mix amount for the saw wave oscillator
    void setSawAmount(float SawAmt)
    {
        SawAmount = SawAmt;
    }
    
    // Sets the amount of vibrato effect applied
    void setVibratoAmount(float VibratoAmt)
    {
        VibratoAmount = VibratoAmt;
    }
    
    // Processes the audio signal, generating the subbass output
    float process()
    {
        // Calculate the vibrato effect
        auto vibratoEffect = vibratoLFO.process() * VibratoAmount; // LFO output modulates around 0
        auto modulatedFrequency = baseFrequency * (1.0f + vibratoEffect);

        // Apply the modulated frequency to the oscillators
        squareOsc.setFrequency(modulatedFrequency);
        sawOsc.setFrequency(modulatedFrequency);
        
        // Generate the waveforms
        auto squareWave = squareOsc.process();
        auto sawWave = sawOsc.process();
        
        // Mix the waveforms and detuned signals
        auto mixedWave = ((squareWave * SquareAmount) + (sawWave * SawAmount) + detuneFine.process() + detuneCoarse.process()) / 4; // Corrected division to include all components equally

        // Process the mixed wave through the filter
        return lowPassFilter.processSingleSampleRaw(mixedWave);
    }
private:
    // Oscillators and LFO
    SquareOsc squareOsc;
    SawOsc sawOsc;
    SawOsc detuneFine;
    SawOsc detuneCoarse;
    SinOsc vibratoLFO;
    
    // Filter
    juce::IIRFilter lowPassFilter;
    
    // Parameters
    float sampleRate = 44100.0f;
    float baseFrequency = 440.0f;
    float VibratoFreq = 5.0f;     // Default Vibrato frequency
    float VibratoAmount = 0.005f; // Default Vibrato amount
    float SquareAmount = 0.7f;    // Default Square Amount
    float SawAmount = 0.7f;       // Default Saw Amount
    float filterCutOff = 400.0f;  // Default cutoff frequency
    int DetuneFine = 5;           // cents, default +5 cents
    int DetuneCoarse = -12;       // Semitones, default -12 Semitones

    // Updates the low-pass filter with a new cutoff frequency
    void updateFilter(float cutoffFrequency)
    {
        lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffFrequency, 5.0));
        lowPassFilter.reset();
    }
};

