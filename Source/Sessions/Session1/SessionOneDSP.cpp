/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionOneDSP.h"

//==============================================================================
const void SessionOneDSP::calculusCoefficients (float frequency, float Q, float gain, 
    int type, double sampleRate,
    float &b0, float &b1, float &b2, float &a1, float &a2)
{
    auto Ts = (float)(1.0 / sampleRate);
    auto w0 = (float)(2.0 * double_Pi * frequency);
    auto A = (float)(Decibels::decibelsToGain (gain / 2.0));

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
        b1 = 0.0f;
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

const double SessionOneDSP::getMagnitudeForFrequency (double frequency, double sampleRate,
    const float b0, const float b1, const float b2, const float a1, const float a2)
{
    constexpr std::complex<double> j (0, 1);
    
    jassert (frequency >= 0 && frequency <= sampleRate * 0.5);

    std::complex<double> numerator = 0.0, denominator = 0.0, factor = 1.0;
    std::complex<double> expjwT = std::exp (-MathConstants<double>::twoPi * frequency * j / sampleRate);

    numerator += ((double)b0) * factor; factor *= expjwT;
    numerator += ((double)b1) * factor; factor *= expjwT;
    numerator += ((double)b2) * factor;    

    denominator = 1.0;
    factor = expjwT;

    denominator += ((double)a1) * factor; factor *= expjwT;
    denominator += ((double)a2) * factor;
    
    return std::abs (numerator / denominator);
}

const double SessionOneDSP::getPhaseForFrequency (double frequency, double sampleRate,
    const float b0, const float b1, const float b2, const float a1, const float a2)
{
    constexpr std::complex<double> j (0, 1);

    jassert (frequency >= 0 && frequency <= sampleRate * 0.5);

    std::complex<double> numerator = 0.0, denominator = 0.0, factor = 1.0;
    std::complex<double> expjwT = std::exp (-MathConstants<double>::twoPi * frequency * j / sampleRate);

    numerator += ((double)b0) * factor; factor *= expjwT;
    numerator += ((double)b1) * factor; factor *= expjwT;
    numerator += ((double)b2) * factor;

    denominator = 1.0;
    factor = expjwT;

    denominator += ((double)a1) * factor; factor *= expjwT;
    denominator += ((double)a2) * factor;

    return std::arg (numerator / denominator);
}
