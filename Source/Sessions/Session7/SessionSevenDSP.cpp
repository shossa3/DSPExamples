/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionSevenDSP.h"


//==============================================================================
SVFLowpassFilterNL::SVFLowpassFilterNL()
{
    frequency.store (1000.f);
    resonance.store (1.f / std::sqrt (2.f));
    sampleRate = 44100.0;
    
    isActive = false;
}

SVFLowpassFilterNL::~SVFLowpassFilterNL()
{

}

void SVFLowpassFilterNL::initProcessing (double _sampleRate)
{
    jassert (_sampleRate > 0);
    
    sampleRate = _sampleRate;

    updateProcessing();
    reset();

    isActive = true;
}

void SVFLowpassFilterNL::setCutoffFrequency (float value)
{
    jassert (value > 0.f && value < 0.5f * sampleRate);
    
    frequency.store (value);
    mustUpdateProcessing = true;
}

void SVFLowpassFilterNL::setResonance (float value)
{
    jassert (value > 0);
    
    resonance.store (1.f / value);
    mustUpdateProcessing = true;
}

void SVFLowpassFilterNL::reset()
{
    s1 = 0.f;
    s2 = 0.f;
}

float SVFLowpassFilterNL::processSingleSampleRaw (const float &sample)
{
    if (!(isActive))
        return 0.f;

    if (mustUpdateProcessing)
        updateProcessing();

    auto yH = (sample - s1 * (R2 + g) - s2) * h;
    auto yB = g * yH + s1;
    auto yL = g * yB + s2;

    yB = std::tanh (yB);
    yL = std::tanh (yL);

    s1 = g * yH + yB;
    s2 = g * yB + yL;

    return yL;
}

void SVFLowpassFilterNL::updateProcessing()
{
    mustUpdateProcessing = false;

    g = std::tan (float_Pi * frequency.load() / (float)sampleRate);
    R2 = resonance.load();
    h = 1.f / (1 + R2 * g + g * g);
}


//==============================================================================
DiodeClipperADC18::DiodeClipperADC18()
{
    frequency.store (1000.f);
    sampleRate = 44100.0;

    isActive = false;
}

DiodeClipperADC18::~DiodeClipperADC18()
{

}

void DiodeClipperADC18::initProcessing (double _sampleRate)
{
    jassert (_sampleRate > 0);

    sampleRate = _sampleRate;

    updateProcessing();
    reset();

    isActive = true;
}

void DiodeClipperADC18::setCutoffFrequency (float value)
{
    jassert (value > 0.f && value < 0.5f * sampleRate);

    frequency.store (value);
    mustUpdateProcessing = true;
}

void DiodeClipperADC18::reset()
{
    s1 = 0.f;
    output = 0.0;
}

float DiodeClipperADC18::processSingleSampleRaw (const float &sample)
{
    if (!(isActive))
        return 0.f;

    if (mustUpdateProcessing)
        updateProcessing();
    
    const auto in = sample;
    const auto p = G * (in - s1) + s1;
    double delta = 1e9;

    // Newton-Raphson iterations
    auto k = 0;

    while (std::abs (delta) > tolerance && k < numIterations)
    {
        auto J = p - (2 * G*R*Is) * std::sinh (output / mu / Vt) - output;
        auto dJ = -1 - G * 2 * R*Is / mu / Vt * std::cosh (output / mu / Vt);
        delta = -J / dJ;

        if (isnan (delta) || isinf (delta) || std::abs (delta) > deltaLim)
            delta = (delta >= 0 ? 1 : -1) * deltaLim;

        output = output + delta;

        k++;
    }

    auto v = (float)output - s1;
    s1 = (float)output + v;

    return (float)output;
}

void DiodeClipperADC18::updateProcessing()
{
    auto g = std::tan (float_Pi * frequency.load() / (float)sampleRate);
    G = g / (1 + g);

    deltaLim = mu * Vt * std::acosh (mu * Vt / 2.0 / (R * Is * G));
}
