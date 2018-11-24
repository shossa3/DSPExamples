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


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
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
