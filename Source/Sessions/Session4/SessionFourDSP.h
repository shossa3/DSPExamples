/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
    This class applies attack / release ballistics on any audio envelope or
    audio metering data, like a diode + R*R*C filter. It is useful for dynamics
    processors and visualization of data.
*/

class BallisticsFilter
{
public:
    //==============================================================================
    /** Constructor. */
    BallisticsFilter();

    /** Destructor. */
    ~BallisticsFilter();

    //==============================================================================
    /** Inits the processing by providing the sample rate. */
    void initProcessing (double sampleRate);

    /** Resets the filter state. */
    void reset (float zeroValue = 0.f);

    /** Processes one sample. */
    float processSingleSample (float &sample);

    //==============================================================================
    /** Sets the attack time in ms. */
    void setAttackTime (float attackTimeMs);

    /** Sets the release time in ms. */
    void setReleaseTime (float releaseTimeMs);

private:
    //==============================================================================
    /** @internal */
    void updateProcessing();

    //==============================================================================
    double sampleRate;
    bool mustUpdateProcessing, isActive;

    //==============================================================================
    std::atomic<float> prmAttack, prmRelease;

    //==============================================================================
    float cteAT, cteRL;
    float yold;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallisticsFilter)
};
