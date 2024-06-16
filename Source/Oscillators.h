//
//  Oscillators.h
//  oscillator class
//
//  Created by 70 on 16/02/2024.
//

#ifndef OSCILLATORS_H  
#define OSCILLATORS_H

#include <JuceHeader.h>
#include <cmath>

// parent class
class Phasor{
    
public:
    virtual ~Phasor() {} // Virtual destructor
    // Our parent oscillator class does the key things required for most oscillators:
    // handles phase
    // handles setters and getters for frequency and samplerate
    // update the phase and output the next sample from the oscillator

    float getPhase()
    {
        updatePhase(); // Update the phase before returning it
        return phase;
    }
    
    float process()
    {
        updatePhase();
        return output(phase);
    }
    
    virtual float output(float p)
    {
        return p;
    }
    
    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }

    void setFrequency(float Freq)
    {
        frequency = Freq;
        phaseDelta = frequency / sampleRate;
    }
    
    
private:
    float frequency = 0.0f;       // Frequency of the oscillator
    float sampleRate = 44100.0f;  // samples per sec
    float phase = 0.0f;           // Current phase of the oscillator
    float phaseDelta = 0.0f;      // Change in phase per sample
    void updatePhase() 
    {
        phase += phaseDelta;
        if (phase > 1.0f)
        {
            phase -= 1.0f;
        }
    }
};
// ==================================

// child class - SinOsc
class SinOsc: public Phasor
{
    float output(float p)override
    {
        return std::sin(p * 2.0 * M_PI);
    }
};

// child class - SquareOsc
class SquareOsc: public Phasor
{
public:
    float output(float p)override
    {
        float outVal = 0.5;
        if(p > pulseWidth)
            outVal = -0.5;
        return outVal;
    }
    void setPulseWidth(float pw)
    {
        pulseWidth = pw;
    }
private:
    float pulseWidth = 0.5f;
};

// child class - TriOsc
class TriOsc: public Phasor
{
    float output(float p)override
    {
        return fabsf(p - 0.5f) - 0.5f;
    }
};

// child class - SawOsc
class SawOsc: public Phasor 
{
public:
    float output(float p)override 
    {
        return p - 0.5f;
    }
};

#endif /* OSCILLATORS_H */
