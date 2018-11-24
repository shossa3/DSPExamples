/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionFiveDSP.h"

//==============================================================================
LinearSlewLimiter::LinearSlewLimiter()
{
    reset();
    setSlew (44100.0, 10000.0, 10000.0);
}

LinearSlewLimiter::~LinearSlewLimiter()
{

}

void LinearSlewLimiter::reset (float resetValue)
{
    lastOutput = resetValue;
    justReset = true;
}

void LinearSlewLimiter::setSlew (double sampleRate, double slewRiseMaximumSlopeInVpS, double slewFallMaximumSlopeInVpS)
{
    slewRiseMax = (float)(slewRiseMaximumSlopeInVpS / sampleRate);
    slewFallMax = (float)(slewFallMaximumSlopeInVpS / sampleRate);
}

float LinearSlewLimiter::getSample (float &input)
{
    if (justReset)
    {
        justReset = false;
        lastOutput = input;
    }
    else
    {
        // Rise limiting
        if (input > lastOutput)
            lastOutput = jmin (input, lastOutput + slewRiseMax);

        // Fall limiting
        else
            lastOutput = jmax (input, lastOutput - slewFallMax);
    }

    return lastOutput;
}

// ===================================================================================================
TPTFilter1stOrder::TPTFilter1stOrder()
{
    sampleRate = 44100;

    type = TypeLowPass;
    frequency = 200;

    isActive = false;
}

TPTFilter1stOrder::~TPTFilter1stOrder()
{

}

void TPTFilter1stOrder::initProcessing (double newSampleRate)
{
    jassert (newSampleRate > 0);

    sampleRate = newSampleRate;

    updateProcessing();
    reset();

    isActive = true;
}

void TPTFilter1stOrder::setType (TPTFilterType newType)
{
    type.store (newType);
    mustUpdateProcessing = true;
}

void TPTFilter1stOrder::setCutoffFrequency (float newFrequency)
{
    jassert (newFrequency > 0);

    newFrequency = jlimit (10.f, static_cast <float> (sampleRate) * 0.5f, newFrequency);

    frequency.store (newFrequency);
    mustUpdateProcessing = true;
}

void TPTFilter1stOrder::reset (const float s1eq)
{
    s1 = s1eq;
}

float TPTFilter1stOrder::processSingleSampleRaw (const float &sample)
{
    if (!(isActive))
        return 0.f;

    if (mustUpdateProcessing)
        updateProcessing();

    auto v = g * (sample - s1);
    auto y = v + s1;
    s1 = y + v;
    
    if (currentType == TypeLowPass)
        return y;
    else
        return sample - y;
}

void TPTFilter1stOrder::updateProcessing()
{
    mustUpdateProcessing = false;

    g = std::tan (float_Pi * frequency.load() / (float)sampleRate);
    g = g / (1 + g);

    currentType = type.load();
}
