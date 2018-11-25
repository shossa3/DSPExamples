/*
  ==============================================================================

  Shaikat Hossain DSPExamples
 
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class LowFrequencyOscillator
{
public:
    // =============================================================================
    /** The different types of LFOs allowed in this class.  */
    enum TypeLFO
    {
        lfoSine = 0,
        lfoTriangle,
        lfoSawtooth,
        lfoSquare,
        lfoNoise
    };

    // =============================================================================
    /** Constructor. */
    LowFrequencyOscillator();

    /** Destructor. */
    ~LowFrequencyOscillator();

    // =============================================================================
    /** Sets the frequency of the LFO. */
    void setFrequency (double newFrequency);

    /** Sets the output volume of the LFO, which is bounded between -1 and +1 when
        this volume is at 0 dB (the parameter is a linear gain value).
    */
    void setVolume (float newVolume);

    /** Sets the type of LFO. */
    void setType (TypeLFO newType);

    // =============================================================================
    /** Initialization of the process. */
    void initProcessing (double newSampleRate);

    /** Resets the LFO and sets the phase to a given value */
    void reset (double initialPhase = 0.0);

    // =============================================================================
    /** Returns next sample. */
    const float getNextSample();

    // =============================================================================
    /** Sets the phase to a given value, for offsetting or synchronizing two LFOs. */
    void copyPhase (double newPhase);

    /** Returns the current phase of the LFO. */
    const double getPhase();

private:
    // =============================================================================
    double sampleRate, frequency;
    int type;

    // =============================================================================
    double factor, phase;
    Random theRandom;

    // =============================================================================
    LinearSmoothedValue<float> outputVolume;
    
    // =============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LowFrequencyOscillator)
};
