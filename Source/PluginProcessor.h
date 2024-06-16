/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "vector"
#include "Oscillators.h"
#include "StringSynth.h"
#include "PadSynth.h"
#include "Movement.h"
#include "Subbass.h"
#include "FrequencySelector.h"

//==============================================================================
/**
*/
class AP_Assignment2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AP_Assignment2AudioProcessor();
    ~AP_Assignment2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    // ============================== processor ====================================
    
    // filter
     float sr; // samplerate
     juce::IIRFilter filter;
    
    // reverb
    juce::Reverb reverb;
    
    // movement(amplitude control)
    Movement movement;
    
    // fade in & out
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedVolume;
    
    // lfos
    SinOsc volLfo;
    SinOsc lfo;
    PadSynth lfo2;
    
    // ============================== timbre ====================================
    
    // StringSynth
    StringSynth string;
    StringSynth stringOctaveUp;
    StringSynth stringRootNote;
    
    // PadSynth
    std::vector<PadSynth> padChords;
    PadSynth leftBounce;
    PadSynth rightBounce;
    PadSynth pad;
    
    // Subbass
    Subbass subbass;
    
    // =========================== FrequencySelector ===========================
    
    FrequencySelector chordsFreqSelector1;
    FrequencySelector chordsFreqSelector2;
    FrequencySelector chordsFreqSelector3;
    FrequencySelector chordsFreqSelector4;
    std::vector<FrequencySelector> chordsFreqSelector;
    
    FrequencySelector leftbounceFreqSelector;
    FrequencySelector rightbounceFreqSelector;
    FrequencySelector stringFreqSelector;
    FrequencySelector padFreqSelector;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AP_Assignment2AudioProcessor)
};
