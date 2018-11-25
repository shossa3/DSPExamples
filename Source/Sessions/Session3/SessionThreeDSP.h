/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class SessionThreeDSP
{
public:
    //==============================================================================
    SessionThreeDSP() {}
    ~SessionThreeDSP() {}

    //==============================================================================
    enum AlgorithmType
    {
        Algorithm_BLT_NoWarping = 0,
        Algorithm_BLT_Warping,
        Algorithm_Forward_Euler,
        Algorithm_Backward_Euler,
        Algorithm_Trap,
        Algorithm_BDF2,
        Algorithm_ImpulseInvariance,
        Algorithm_Massberg,
        Algorithm_Vicanek
    };

    //==============================================================================
    static const void calculusLP2Coefficients (float frequency, float Q,
        int algorithm, double sampleRate, 
        int &order,
        float &b0, float &b1, float &b2, float &b3, float &b4,
        float &a1, float &a2, float &a3, float &a4);

    static const double getMagnitudeForFrequency (double frequency, double sampleRate,
        const float b0, const float b1, const float b2, const float b3, const float b4,
        const float a1, const float a2, const float a3, const float a4 );

    static const double getPhaseForFrequency (double frequency, double sampleRate,
        const float b0, const float b1, const float b2, const float b3, const float b4,
        const float a1, const float a2, const float a3, const float a4);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionThreeDSP)
};
