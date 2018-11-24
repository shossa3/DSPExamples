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
class SessionOneDSP
{
public:
    //==============================================================================
    SessionOneDSP() {}
    ~SessionOneDSP() {}

    //==============================================================================
    enum FilterType
    {
        Type_LowPass1 = 0,
        Type_HighPass1,
        Type_LowPass2,
        Type_BandPass2,
        Type_HighPass2,
        Type_Peak
    };

    //==============================================================================
    static const void calculusCoefficients (float frequency, float Q, float gain, 
        int type, double sampleRate, 
        float &b0, float &b1, float &b2, float &a1, float &a2);

    static const double getMagnitudeForFrequency (double frequency, double sampleRate,
        const float b0, const float b1, const float b2, const float a1, const float a2);

    static const double getPhaseForFrequency (double frequency, double sampleRate,
        const float b0, const float b1, const float b2, const float a1, const float a2);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionOneDSP)
};
