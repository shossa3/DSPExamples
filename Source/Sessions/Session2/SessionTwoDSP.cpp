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

#include "SessionTwoDSP.h"

//==============================================================================
const void SessionTwoDSP::calculusAnalogCoefficients (float frequency, float Q, float gain,
    int type,
    float &b0, float &b1, float &b2, float &a1, float &a2)
{
    auto w0 = (float)(2.0 * double_Pi * frequency);
    auto A = (float)(Decibels::decibelsToGain (gain / 2.0));
    
    if (type == Type_LowPass1)
    {
        auto a0 = 1.0f;

        b0 = 1.0f / a0;
        b1 = 0.0f;
        b2 = 0.0f;

        a1 = (1.0f / w0) / a0;
        a2 = 0.0f;
    }
    else if (type == Type_HighPass1)
    {
        auto a0 = 1.0f;

        b0 = 0.f;
        b1 = (1.0f / w0) / a0;
        b2 = 0.0f;

        a1 = (1.0f / w0) / a0;
        a2 = 0.0f;
    }
    else if (type == Type_LowPass2)
    {
        auto a0 = 1.f;

        b0 = 1.f / a0;
        b1 = 0.f;
        b2 = 0.f;

        a1 = (1.0f / (Q * w0)) / a0;
        a2 = (1.0f / (w0 * w0)) / a0;
    }
    else if (type == Type_BandPass2)
    {
        auto a0 = 1.f;

        b0 = 0.f;
        b1 = (1.0f / (Q * w0)) / a0;
        b2 = 0.f;

        a1 = (1.0f / (Q * w0)) / a0;
        a2 = (1.0f / (w0 * w0)) / a0;
    }
    else if (type == Type_HighPass2)
    {
        auto a0 = 1.f;

        b0 = 0.f;
        b1 = 0.f;
        b2 = (1.0f / (w0 * w0)) / a0;

        a1 = (1.0f / (Q * w0)) / a0;
        a2 = (1.0f / (w0 * w0)) / a0;
    }
    else if (type == Type_Peak)
    {
        auto a0 = 1.f;

        b0 = 1.0f / a0;
        b1 = (A / (Q * w0)) / a0;
        b2 = (1.0f / w0 / w0) / a0;

        a1 = (1.0f / (A * Q * w0)) / a0;
        a2 = (1.0f / (w0 * w0)) / a0;
    }
    else
    {
        b0 = 1.0f; b1 = 0.0f; b2 = 0.0f;
        a1 = 0.0f; a2 = 0.0f;
    }
}

const void SessionTwoDSP::calculusPreWarpedCoefficients (float frequency, float Q, float gain, 
    int type, double _sampleRate,
    float &b0, float &b1, float &b2, float &a1, float &a2)
{
    auto Ts = (float)(1.0 / _sampleRate);
    auto w0 = (float)(2.0 * double_Pi * frequency);
    auto A = (float)(Decibels::decibelsToGain (gain / 2.0));

    Ts = (float) (std::tan (w0 * Ts / 2.0) * 2.0 / w0);

    auto Ts2 = Ts * Ts;
    auto w02 = w0 * w0;
    auto A2 = A * A;

    if (type == Type_LowPass1)
    {
        auto a0 = Ts * w0 + 2.0f;

        b0 = (Ts * w0) / a0;
        b1 = (Ts * w0) / a0;
        b2 = 0.0f;

        a1 = (Ts * w0 - 2.0f) / a0;
        a2 = 0.0f;
    }
    else if (type == Type_HighPass1)
    {
        auto a0 = Ts * w0 + 2.0f;

        b0 = 2.0f / a0;
        b1 = -2.0f / a0;
        b2 = 0.0f;

        a1 = (Ts * w0 - 2.0f) / a0;
        a2 = 0.0f;
    }
    else if (type == Type_LowPass2)
    {
        auto a0 = Q * Ts2 * w02 + 2 * Ts * w0 + 4 * Q;

        b0 = (Q * Ts2 * w02) / a0;
        b1 = (2 * Q * Ts2 * w02) / a0;
        b2 = (Q * Ts2 * w02) / a0;

        a1 = (2 * Q * Ts2 * w02 - 8 * Q) / a0;
        a2 = (Q * Ts2 * w02 - 2 * Ts * w0 + 4 * Q) / a0;
    }
    else if (type == Type_BandPass2)
    {
        auto a0 = Q * Ts2 * w02 + 2 * Ts * w0 + 4 * Q;

        b0 = (2 * Ts * w0) / a0;
        b1 = 0.0;
        b2 = (-2 * Ts * w0) / a0;

        a1 = (2 * Q * Ts2 * w02 - 8 * Q) / a0;
        a2 = (Q * Ts2 * w02 - 2 * Ts * w0 + 4 * Q) / a0;
    }
    else if (type == Type_HighPass2)
    {
        auto a0 = Q * Ts2 * w02 + 2 * Ts * w0 + 4 * Q;

        b0 = (4 * Q) / a0;
        b1 = (-8 * Q) / a0;
        b2 = (4 * Q) / a0;

        a1 = (2 * Q * Ts2 * w02 - 8 * Q) / a0;
        a2 = (Q * Ts2 * w02 - 2 * Ts * w0 + 4 * Q) / a0;
    }
    else if (type == Type_Peak)
    {
        auto a0 = A * Q * Ts2 * w02 + 2 * Ts * w0 + 4 * A * Q;

        b0 = (2 * A2 * Ts * w0 + Q * A * Ts2 * w02 + Q * A * 4) / a0;
        b1 = (2 * A * Q * Ts2 * w02 - 8 * A * Q) / a0;
        b2 = (-2 * A2 * Ts * w0 + Q * A * Ts2 * w02 + Q * A * 4) / a0;

        a1 = (2 * A * Q * Ts2 * w02 - 8 * A * Q) / a0;
        a2 = (A * Q * Ts2 * w02 - 2 * Ts * w0 + 4 * A * Q) / a0;
    }
    else
    {
        b0 = 1.0f; b1 = 0.0f; b2 = 0.0f;
        a1 = 0.0f; a2 = 0.0f;
    }
}

const double SessionTwoDSP::getAnalogMagnitudeForFrequency (double frequency,
    const float b0, const float b1, const float b2, const float a1, const float a2)
{
    constexpr std::complex<double> j (0, 1);
    
    jassert (frequency >= 0);

    std::complex<double> numerator = 0.0, denominator = 0.0, factor = 1.0;
    std::complex<double> jw = MathConstants<double>::twoPi * frequency * j;

    numerator += ((double)b0) * factor; factor *= jw;
    numerator += ((double)b1) * factor; factor *= jw;
    numerator += ((double)b2) * factor;    

    denominator = 1.0;
    factor = jw;

    denominator += ((double)a1) * factor; factor *= jw;
    denominator += ((double)a2) * factor;
    
    return std::abs (numerator / denominator);
}

const double SessionTwoDSP::getAnalogPhaseForFrequency (double frequency,
    const float b0, const float b1, const float b2, const float a1, const float a2)
{
    constexpr std::complex<double> j (0, 1);

    jassert (frequency >= 0);

    std::complex<double> numerator = 0.0, denominator = 0.0, factor = 1.0;
    std::complex<double> jw = MathConstants<double>::twoPi * frequency * j;

    numerator += ((double)b0) * factor; factor *= jw;
    numerator += ((double)b1) * factor; factor *= jw;
    numerator += ((double)b2) * factor;

    denominator = 1.0;
    factor = jw;

    denominator += ((double)a1) * factor; factor *= jw;
    denominator += ((double)a2) * factor;

    return std::arg (numerator / denominator);
}
