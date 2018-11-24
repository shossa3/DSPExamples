/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
    Linear slew limiter class
*/
class LinearSlewLimiter
{
public:
    //==============================================================================
    LinearSlewLimiter();
    ~LinearSlewLimiter();
    
    //==============================================================================
    void reset (float resetValue = 0.f);
    void setSlew (double sampleRate, double slewRiseMaximumSlopeInVpS, double slewFallMaximumSlopeInVpS);
    
    //==============================================================================
    float getSample (float &input);

private:
    //==============================================================================
    float slewRiseMax, slewFallMax;
    float lastOutput;
    bool justReset;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinearSlewLimiter)
};


//==============================================================================
/**
    An IIR filter that can perform low, and high-pass filtering on an audio
    signal, with 6 dB of attenuation / octave, using a TPT structure, designed
    for fast modulation (see Vadim Zavalishin's documentation about TPT
    structures for more information).
*/
class TPTFilter1stOrder
{
public:
    enum TPTFilterType
    {
        TypeLowPass = 0,
        TypeHighPass
    };

    //==============================================================================
    /** Creates a filter.

        Initially the filter is inactive, so will have no effect on samples that
        you process with it. Use the initProcessing and set methods to turn it into
        the type of filter needed.
    */
    TPTFilter1stOrder();

    /** Destructor. */
    ~TPTFilter1stOrder();

    //==============================================================================
    /** Initialization of the filter */
    void initProcessing (double sampleRate);

    /** Sets the type of IIR filter */
    void setType (TPTFilterType type);

    /** Sets the cutoff frequency of the IIR filter */
    void setCutoffFrequency (float frequency);

    //==============================================================================
    /** Resets the filter's processing pipeline. */
    void reset (const float s1eq = 0.f);

    /** Processes a single sample. */
    float processSingleSampleRaw (const float &sample);
        
private:
    //==============================================================================
    /** Updates the internal variables */
    void updateProcessing();
    
    //==============================================================================
    double sampleRate;
    std::atomic <int> type;
    std::atomic <float> frequency;

    float g, s1;
    int currentType = 0;
    
    bool isActive, mustUpdateProcessing;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TPTFilter1stOrder)
};
