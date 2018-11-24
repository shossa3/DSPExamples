/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../Session4/SessionFourDSP.h"
#include "../Session5/SessionFiveDSP.h"


//==============================================================================
/**
    Envelope follower class.

    This class provides a process which takes a given input signal, and returns
    another signal with a value between 0 and 1, meant to modulate anything such
    as a filter cutoff frequency, a volume control or any other parameter in an
    audio effect or a synthesizer.

    The API provides different ways to customize the behaviour of the envelope
    signal generated, such as pre and post processing volume, ballistics filters
    time etc.
*/

class EnvelopeFollower
{
public:
    //==============================================================================
    /** Constructor. */
    EnvelopeFollower();

    /** Destructor. */
    ~EnvelopeFollower();

    //==============================================================================
    /** Inits the processing by providing the sample rate. */
    void initProcessing (double sampleRate);

    /** Resets the filter state. */
    void reset (float zeroValue = 0.f);

    /** Processes one sample. */
    float processSingleSample (float sample);

    //==============================================================================
    /** Sets a volume to apply before any processing to the sidechain signal. */
    void setPreGain (float gaindB);

    /** Sets a volume to apply to the signal after processing */
    void setVolume (float volumedB);

    /** Sets the attack time in the ballistics filter. */
    void setAttackTime (float attackTimeMs);

    /** Sets the release time in the ballistics filter. */
    void setReleaseTime (float releaseTimeMs);

private:
    //==============================================================================
    /** @internal */
    void updateProcessing();

    //==============================================================================
    double sampleRate;
    bool mustUpdateProcessing, isActive;

    //==============================================================================
    std::atomic<float> prmGain, prmOutput;
    std::atomic<float> prmAttack, prmRelease;

    //==============================================================================
    BallisticsFilter theBallisticsFilter;
    LinearSmoothedValue<float> gainVolume, outputVolume;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeFollower)
};
