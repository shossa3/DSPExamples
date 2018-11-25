/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


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



class TPTFilter1stOrder
{
public:
    enum TPTFilterType
    {
        TypeLowPass = 0,
        TypeHighPass
    };


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
