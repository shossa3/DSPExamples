/*
  ==============================================================================

   This file is part of the DSP Workshop #2 happening at ADC18
   Copyright (c) 2018 - Ivan COHEN

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


// =============================================================================
/**
    Low Frequency Oscillator class, which can be used to modulate any parameter
    in an audio effect or a synthesizer.

    It is very simple, and not meant to be treated as a regular oscillator class,
    since it doesn't feature any anti-aliasing system.

    A slew limiter is provided as well (first order filter, log) to allow
    optional customization of curves, as well as a sample and hold system.
*/
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