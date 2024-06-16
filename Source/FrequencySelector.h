/*
  ==============================================================================

    FrequencySelector.h
    Created: 3 Mar 2024 2:49:27pm
    Author:  70

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include <vector>

/**
    Manages dynamic frequency selection from a predefined list for audio applications.

    The FrequencySelector allows for either random or sequential selection of frequencies, suitable for
    creating varied audio outputs in applications such as synthesizers and audio testing tools. Configure
    it by setting a sample rate, a list of frequencies, a hold duration for each frequency, and the selection
    mode. Use process() to retrieve the current frequency based on the configured parameters and selection
    logic.
 */
class FrequencySelector
{
public:

    // Defines the selection mode as either random or sequential
    enum class SelectionMode 
    {
        Random,    // Frequencies are chosen randomly
        Sequential // Frequencies are chosen in the order they appear in the list
    };

    // Holds configuration parameters for the FrequencySelector
    struct Parameters 
    {
        float sampleRate = 44100.0f;                 // The audio sample rate in Hz, defaulting to 44.1kHz.
        std::vector<float> frequencies = {440.0f};   // A list of frequencies to select from, defaulting to A4.
        float holdDuration = 1.0f;                   // The duration to hold a selected frequency in seconds.
        SelectionMode mode = SelectionMode::Random;  // The mode of frequency selection, defaulting to random.
    };

    // Constructor that initializes the FrequencySelector with default parameters
    FrequencySelector()
    {
        setParameters(Parameters());
    }

    // Returns a constant reference to the current parameters.
    const Parameters& getParameters() const noexcept { return parameters; }

    // Sets the parameters for frequency selection and updates the internal state accordingly.
    void setParameters(const Parameters& newParams) 
    {
        parameters = newParams;
        samplesUntilNextFrequency = int(parameters.sampleRate * parameters.holdDuration);
        sequenceIndex = 0; // Reset sequence index on parameter change
        updateFrequency(); // Update frequency with new parameters
    }

    // Processes a single sample, updating the frequency selection as necessary
    float process()
    {
        if (++samplesPlayed >= samplesUntilNextFrequency)
            updateFrequency();
        return currentFrequency;
    }

private:
    Parameters parameters;                 // Holds the current selection parameters.
    int samplesUntilNextFrequency = 0;     // The number of samples to play before selecting a new frequency.
    int samplesPlayed = 0;                 // The number of samples that have been played since the last update.
    float currentFrequency = 440.0f;       // The current frequency being output.
    unsigned int sequenceIndex = 0;        // The index for the next frequency in sequential mode.

    // Updates the current frequency based on the selection mode and parameters.
    void updateFrequency()
    {
        if (parameters.frequencies.empty()) return;

        switch (parameters.mode) 
        {
            case SelectionMode::Random: 
            {
                auto randomIndex = juce::Random::getSystemRandom().nextInt(int(parameters.frequencies.size()));
                currentFrequency = parameters.frequencies[randomIndex]; // Select a random frequency
                break;
            }
            case SelectionMode::Sequential: 
            {
                currentFrequency = parameters.frequencies[sequenceIndex++]; // Select the next frequency in sequence
                if (sequenceIndex >= parameters.frequencies.size()) sequenceIndex = 0; // Loop back to the start
                break;
            }
        }

        samplesPlayed = 0;
    }
};


