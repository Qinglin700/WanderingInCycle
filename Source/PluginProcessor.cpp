/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include "vector"

//==============================================================================
AP_Assignment2AudioProcessor::AP_Assignment2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

AP_Assignment2AudioProcessor::~AP_Assignment2AudioProcessor()
{
}

//==============================================================================
const juce::String AP_Assignment2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AP_Assignment2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AP_Assignment2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AP_Assignment2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AP_Assignment2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AP_Assignment2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AP_Assignment2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void AP_Assignment2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AP_Assignment2AudioProcessor::getProgramName (int index)
{
    return {};
}

void AP_Assignment2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AP_Assignment2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // ============================== processor ====================================
    // filter
    filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, 300.0));
    filter.reset();
    sr = sampleRate;
    
    // reverb
    juce::Reverb::Parameters reverbParams;
    reverbParams.dryLevel = 0.5f;
    reverbParams.wetLevel = 0.05f;
    reverbParams.roomSize = 0.9f;
    reverb.setParameters(reverbParams);
    reverb.reset();
    
    // movement(amplitude control)
    movement.setSampleRate(sampleRate);
    movement.setFrequency(5.0);
    movement.setVibratoFreq(1.0f);
    
    // fade in
    smoothedVolume.reset(sampleRate, 2.0);
    smoothedVolume.setTargetValue(1.0f); // Start fully faded in.
    
    // LFOs to control parameters
    lfo.setSampleRate(sampleRate);
    lfo.setFrequency(0.05);
    lfo2.setSampleRate(sampleRate);
    lfo2.setFrequency(0.1);
    volLfo.setSampleRate(sampleRate);
    volLfo.setFrequency(1.0);
    
    // ============================== timbre ====================================
    
    // StringSynth
    string.setSampleRate(sampleRate);
    string.setFrequency(294);
    string.setVibratoFreq(5.0f);
    
    stringOctaveUp.setSampleRate(sampleRate);
    stringOctaveUp.setFrequency(294);
    stringOctaveUp.setVibratoFreq(5.0f);
    
    stringRootNote.setSampleRate(sampleRate);
    stringRootNote.setFrequency(294);
    stringRootNote.setVibratoFreq(0.0f);
    
    // PadSynth
    // 1. Pad chords (vector)
    int oscCount = 4;
    padChords.clear();
    for (int i = 0; i < oscCount; i++)
    {
        padChords.push_back(PadSynth());
        padChords[i].setSampleRate(sampleRate);
    }
    
    // 2. Bounce
    leftBounce.setSampleRate(sampleRate);
    leftBounce.setFrequency(294);
    leftBounce.setLFOFrequency(5.0f);

    rightBounce.setSampleRate(sampleRate);
    rightBounce.setFrequency(294);
    rightBounce.setLFOFrequency(3.0f);
    
    // 3. pad embellishment in high frequency
    pad.setSampleRate(sampleRate);
    pad.setFrequency(440);
    pad.setLFOFrequency(0.0f);
    
    // Subbass
    subbass.setSampleRate(sampleRate);
    subbass.setFrequency(82.41);
    subbass.setVibratoFreq(1.0f);
    
    // =========================== FrequencySelector ===========================
    // chordsFrequencySelector
   
    // Determine the number of FrequencySelector
    int selectorCount = 4;
    chordsFreqSelector.clear();
    for (int i = 0; i < selectorCount; i++)
    {
        chordsFreqSelector.push_back(FrequencySelector());
    }
    
    // Initialize a vector to hold the parameters for each chord's FrequencySelector.
    int paramsCount = 4;
    std::vector<FrequencySelector::Parameters> chordsFreqParams(paramsCount);
    
    // Configure common parameters for all chords
    for (int i = 0; i < 4; i++)
    {
        chordsFreqParams[i].sampleRate = sampleRate;
        chordsFreqParams[i].holdDuration = 6.4f;
        chordsFreqParams[i].mode = FrequencySelector::SelectionMode::Sequential; // Use sequential mode
    }
    
    // Set the specific frequencies for each chord
    // Fmaj7 (F4, A4, C4, E4) - Dm7 (D4, F4, A4, C4) - Am7 (A3, C4, E4, G4) - Em7 (E4, G4, B3, D4)
    chordsFreqParams[0].frequencies = {349.23, 293.66, 220.00, 329.63}; // Root notes
    chordsFreqParams[1].frequencies = {440.00, 349.23, 261.63, 392.00}; // Thirds
    chordsFreqParams[2].frequencies = {261.63, 440.00, 329.63, 246.94}; // Fifths
    chordsFreqParams[3].frequencies = {329.63, 261.63, 392.00, 293.66}; // Sevenths
    
    // Apply the configured parameters to each corresponding FrequencySelector
    for (int i = 0; i < 4; i++)
    {
        chordsFreqSelector[i].setParameters(chordsFreqParams[i]);
    }
    
    
    // bounceFrequencySelector
    FrequencySelector::Parameters leftbounceFreqParams;
    leftbounceFreqParams.sampleRate = sampleRate;
    leftbounceFreqParams.frequencies = {261.63, 329.63, 392.00, 0.00, 0.00}; // 0 is used to create an interval
    leftbounceFreqParams.holdDuration = 6.4f;
    leftbounceFreqSelector.setParameters(leftbounceFreqParams); // Default random mode
  
    FrequencySelector::Parameters rightbounceFreqParams;
    rightbounceFreqParams.sampleRate = sampleRate;
    rightbounceFreqParams.frequencies = {261.63, 329.63, 392.00, 0.00, 0.00}; // 0 is used to create an interval
    rightbounceFreqParams.holdDuration = 3.2f;
    rightbounceFreqSelector.setParameters(rightbounceFreqParams); // Default random mode
    
    
    // stringFrequencySelector
    FrequencySelector::Parameters stringFreqParams;
    stringFreqParams.sampleRate = sampleRate;
    stringFreqParams.frequencies = {261.63, 261.63, 261.63, 0.00, 392.00, 349.23, 329.63, 0.00}; // 0 is used to create an interval
    stringFreqParams.holdDuration = 0.8f;
    stringFreqParams.mode = FrequencySelector::SelectionMode::Sequential; // Use sequential mode
    stringFreqSelector.setParameters(stringFreqParams);
    
    
    // padFrequencySelector
    FrequencySelector::Parameters padFreqParams;
    padFreqParams.sampleRate = sampleRate;
    padFreqParams.frequencies = {1046.52, 1568, 0.00, 0.00, 0.00, 0.00, 0.00}; // 0 is used to create an interval
    padFreqParams.holdDuration = 0.4f;
    padFreqSelector.setParameters(padFreqParams); // Default random mode
}

void AP_Assignment2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AP_Assignment2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AP_Assignment2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0); // left channel
    float* rightChannel = buffer.getWritePointer(1); // right channel
    
    // DSP loop
    for(int i = 0; i < numSamples; i++)
    {
        // === Amplitude Control ===
        // Modulate the amplitude of a signal using the instance from "movement" class
        float vibratoAmount = (lfo.process() + 1.0f) / 80 + 0.005; // Scale LFO output from -1~1 to 0.005~0.03
        movement.setVibratoAmount(vibratoAmount);
        auto movementVal = movement.process();
        
        // === Stereo Volume Control ===
        // Control the volume of the left and right channels independently to create a stereo effect.
        float leftVolume = (volLfo.process() + 1.0f) / 2; // scale it from -1-1 to 0-1
        float rightVolume = 1 - leftVolume;
        
        
        // === Pad Synthesis ===
        // 1. Pad chords (vector)
        float outputValue = 0.0; // Initialize the output value
        
        // Set frequencies for each pad oscillator, selected by chord frequency selectors.
        float firstFrequency = chordsFreqSelector[0].process();
        padChords[0].setFrequency(firstFrequency);
        float secondFrequency = chordsFreqSelector[1].process();
        padChords[1].setFrequency(secondFrequency);
        float thirdFrequency = chordsFreqSelector[2].process();
        padChords[2].setFrequency(thirdFrequency);
        float fourthFrequency = chordsFreqSelector[3].process();
        padChords[3].setFrequency(fourthFrequency);
        
        // Set dynamic parameters for Pad chords
        float filterCutoffVal = lfo.process() * 4000 + 5000;
        float LFOFreq = (lfo.process() + 1.0f) * 3 + 0.1; // scale it from -1-1 to 0.1-6
        float LFOAmount = (lfo.process() + 1.0f) / 8; // scale it from -1-1 to 0.0-0.25
        
        // Generate the waveforms
        for (int j = 0; j < padChords.size(); j++)
        {
            padChords[j].setFilterCutOff(filterCutoffVal);
            padChords[j].setLFOFrequency(LFOFreq);
            padChords[j].setLFOAmount(LFOAmount);
            outputValue += padChords[j].process();
        }
        float padchordsSamples = outputValue / padChords.size();

        // 2. Bounce
        // apply high pass filter to reduce low frequency
        float cutoff = lfo.process() * 1000.0f + 2000.0f; // moving filter
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, cutoff, 5.0f));
        
        // select notes
        float leftbounceFreq = leftbounceFreqSelector.process();
        float rightbounceFreq = rightbounceFreqSelector.process();
        
        // Set frequencies selected by frequency selectors
        leftBounce.setFrequency(leftbounceFreq);
        rightBounce.setFrequency(rightbounceFreq);
        
        // add dynamic timbre change
        float LFOBounceFreq = (lfo2.process() + 1.0f) * 2.5 + 0.1; // scale it from -1-1 to 0.1-5
        leftBounce.setLFOFrequency(LFOBounceFreq);
        rightBounce.setLFOFrequency(LFOBounceFreq);
        
        // Generate the raw waveforms
        auto leftBouncerawSamples = leftBounce.process();
        auto rightBouncerawSamples = rightBounce.process();
        
        // Process through the filter and add movement
        auto leftBounceSamples = filter.processSingleSampleRaw(leftBouncerawSamples) * movementVal;
        auto rightBounceSamples = filter.processSingleSampleRaw(rightBouncerawSamples) * movementVal;
        
        // 3. Pad embellishment in high frequency
        float padFrequency = padFreqSelector.process(); // select notes
        pad.setFrequency(padFrequency); // Set frequencies selected by frequency selectors
        auto padSamples = pad.process(); // Generate the waveforms
        auto leftpadSamples = padSamples * leftVolume * 0.1; // panning and reduce the volume
        auto rightpadSamples = padSamples * rightVolume * 0.1; // panning and reduce the volume
        

        // === String Synthesis ===
        // 1. root note
        stringRootNote.setFrequency(firstFrequency / 2);   // add string to emphasize the root note
        auto stringRootVol =  (lfo2.process() + 1.0f) / 3; // scale it from -1-1 to 0-0.5
        auto stringRootSA =  (lfo2.process() + 1.0f) / 2;  // scale it from -1-1 to 0.5-1
        stringRootNote.setSawAmount(stringRootSA);         // add dynamic timbre change
        auto stringRootSamples = stringRootNote.process() * stringRootVol;
        
        // 2. motif
        float stringFrequency = stringFreqSelector.process(); // select notes
        auto stringOctUpSA =  (lfo.process() + 1.0f) / 4 + 0.5; // scale it from -1-1 to 0.5-1
        string.setFrequency(stringFrequency); // Set frequencies selected by frequency selectors
        stringOctaveUp.setFrequency(stringFrequency * 2); // enrich timbre
        stringOctaveUp.setSawAmount(stringOctUpSA); // add dynamic timbre change
        auto stringSamples = (string.process() + stringOctaveUp.process() * 0.9) / 2; // scale it to normal level
       
        
        // === Sub Bass Synthesis ===
        float subPulseWidth = (lfo2.process() + 1.0f) / 4; // scale it from -1-1 to 0-0.5
        float squareAmount = (lfo.process() + 1.0f) / 4 + 0.25; // scale it from -1-1 to 0.25-0.75
        int detuneFine = int (lfo.process() * 30); // scale it from -1-1 to -30-30
        subbass.setSquarePulseWidth(subPulseWidth); // add dynamic timbre change
        subbass.setDetuneFine(detuneFine); // add dynamic timbre change
        subbass.setSquareAmount(squareAmount); // add dynamic timbre change
        auto subbassSamples = subbass.process() * (movementVal * 0.5 + 0.5) ; // add subtle movement
        
        // === Final Mix ===
        // center(mono) - final mix
        auto mixsamples = (padchordsSamples + stringSamples + subbassSamples * 0.3 + stringRootSamples) / 2;
        
        // stereo - final mix
         auto mixL = (mixsamples + leftBounceSamples * 0.4 + leftpadSamples) * smoothedVolume.getNextValue();
         auto mixR = (mixsamples + rightBounceSamples * 0.4 + rightpadSamples) * smoothedVolume.getNextValue();
        
        // final output
        leftChannel[i] = mixL;
        rightChannel[i] = mixR;
    }
    // Apply stereo reverb to the final mix
    reverb.processStereo(leftChannel, rightChannel, numSamples);
}

//==============================================================================
bool AP_Assignment2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AP_Assignment2AudioProcessor::createEditor()
{
    return new AP_Assignment2AudioProcessorEditor (*this);
}

//==============================================================================
void AP_Assignment2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AP_Assignment2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AP_Assignment2AudioProcessor();
}
