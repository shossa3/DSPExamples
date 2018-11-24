/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
    State Variable Filter (low-pass output) with TPT Structure and naive tanh
    nonlinearity
*/
class SVFLowpassFilterNL
{
public:
    //==============================================================================
    SVFLowpassFilterNL();
    ~SVFLowpassFilterNL();

    //==============================================================================
    /** Initialization of the filter */
    void initProcessing (double sampleRate);

    /** Sets the cutoff frequency of the filter */
    void setCutoffFrequency (float value);

    /** Sets the resonance of the filter */
    void setResonance (float value);

    //==============================================================================
    /** Resets the filter's processing pipeline. */
    void reset();

    /** Processes a single sample. */
    float processSingleSampleRaw (const float &sample);

private:
    //==============================================================================
    /** Updates the internal variables */
    void updateProcessing();

    //==============================================================================
    double sampleRate;

    std::atomic <float> frequency;
    std::atomic <float> resonance;

    float g, R2, h;
    float s1, s2;

    bool isActive, mustUpdateProcessing;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVFLowpassFilterNL)
};



//==============================================================================
/**
    Diode clipper algorithm with TPT structure and Newton-Raphson's method to
    solve the nonlinearities
*/
class DiodeClipperADC18
{
public:
    //==============================================================================
    DiodeClipperADC18();
    ~DiodeClipperADC18();

    //==============================================================================
    /** Initialization of the filter */
    void initProcessing (double sampleRate);

    /** Sets the cutoff frequency of the filter */
    void setCutoffFrequency (float value);
        
    //==============================================================================
    /** Resets the filter's processing pipeline. */
    void reset();

    /** Processes a single sample. */
    float processSingleSampleRaw (const float &sample);

private:
    //==============================================================================
    /** Updates the internal variables */
    void updateProcessing();

    //==============================================================================
    double sampleRate;

    std::atomic<float> frequency;
    
    float G, R2, h;
    float s1;
    double output;
    double deltaLim;

    bool isActive, mustUpdateProcessing;

    //==============================================================================
    // Constants
    const int numIterations = 8;
    const double Is = 14.11e-9;
    const double mu = 1.984;
    const double Vt = 26e-3;
    const double R = 2.2e3;
    const double tolerance = 1e-12;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DiodeClipperADC18)
};
