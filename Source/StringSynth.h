//
//  StringSynth.h
//

#ifndef StringSynth_h
#define StringSynth_h

#include "Oscillators.h"
#include <JuceHeader.h>

/**
    Crafts rich, resonant string sounds with vibrato and low-pass filtering.

    StringSynth combines square and saw oscillators, enhanced by vibrato via an LFO, and shaped with a low-pass filter for classic string timbres. It offers detailed control over oscillator mix, pulse width, and vibrato depth. Initialize with setSampleRate(), customize the sound with oscillator and filter settings, then use process() to generate the output. Ideal for emulating vintage string machines and creating modern string sounds.
 */
class StringSynth
{
public:
    StringSynth() 
    {
        updateFilter(filterCutoff); // Initialize the filter with the default cutoff frequency
    }
    
    // Sets the sample rate for the internal oscillators and updates the filter
    void setSampleRate(float SR)
    {
        sampleRate = SR;
        squareOsc.setSampleRate(sampleRate);
        sawOsc.setSampleRate(sampleRate);
        vibratoLFO.setSampleRate(sampleRate);
    }

    // Sets the base frequency for the oscillators
    void setFrequency(float Freq)
    {
        baseFrequency = Freq;
        squareOsc.setFrequency(baseFrequency);
        sawOsc.setFrequency(baseFrequency);
    }

    // Sets the vibrato LFO frequency
    void setVibratoFreq(float VF) // sin LFO
    {
        VibratoFreq = VF;
        vibratoLFO.setFrequency(VibratoFreq);
    }
    
    // Sets the pulse width for the square oscillator
    void setSquarePulseWidth(float PW)
    {
        pulseWidth = PW;
        squareOsc.setPulseWidth(pulseWidth);
    }

    // Sets the mix amount for the square oscillator
    void setSquareAmount(float SquareAmt)
    {
        SquareAmount = SquareAmt;
    }
    
    // Sets the mix amount for the saw oscillator
    void setSawAmount(float SawAmt)
    {
        SawAmount = SawAmt;
    }
    
    // Sets the amount of vibrato modulation
    void setVibratoAmount(float VibratoAmt)
    {
        VibratoAmount = VibratoAmt;
    }
    
    // Processes the audio signal, applying vibrato and filtering
    float process()
    {
        // Calculate the vibrato effect
        //auto vibratoVal = vibratoLFO.process();
        auto vibratoEffect = vibratoLFO.process() * VibratoAmount; // LFO output modulates around 0
        auto modulatedFrequency = baseFrequency * (1.0f + vibratoEffect);

        // Apply the modulated frequency to the oscillators
        squareOsc.setFrequency(modulatedFrequency);
        sawOsc.setFrequency(modulatedFrequency);
        
        // Generate the waveforms
        auto squareWave = squareOsc.process();
        auto sawWave = sawOsc.process();
        
        // Mix the two waveforms. Adjust the mixing ratio as needed.
        auto mixedWave = (squareWave * SquareAmount) + (sawWave * SawAmount);

        // Process the mixed wave through the filter
        return lowPassFilter.processSingleSampleRaw(mixedWave);
    }
private:
    SquareOsc squareOsc;
    SawOsc sawOsc;
    SinOsc vibratoLFO;
    juce::IIRFilter lowPassFilter;
    
    float sampleRate = 44100.0f;
    float baseFrequency = 440.0f;
    float VibratoFreq = 5.0f;     // Default Vibrato frequency
    float VibratoAmount = 0.005f; // Default Vibrato amount
    float pulseWidth = 0.5f;
    float SquareAmount = 0.5f;    // Default Square Amount
    float SawAmount = 1.0f;       // Default Saw Amount
    float filterCutoff = 1200.0f; // Default cutoff frequency
    
    // Updates the low-pass filter's coefficients based on the current cutoff frequency
    void updateFilter(float cutoffFrequency)
    {
        lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffFrequency));
        lowPassFilter.reset();
    }
};

#endif /* StringSynth_h */
