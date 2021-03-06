/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "LowFrequencyOscillator.h"



// =========================================================================
LowFrequencyOscillator::LowFrequencyOscillator()
{
    sampleRate = 44100;

    setFrequency (1.0);
    setType (TypeLFO::lfoSine);
    setVolume (1.f);

    theRandom.setSeedRandomly();

    outputVolume.setValue (1.f);
}

LowFrequencyOscillator::~LowFrequencyOscillator()
{

}

void LowFrequencyOscillator::setFrequency (double newFrequency)
{
    frequency = newFrequency;
    factor = 2.0 * double_Pi * newFrequency / sampleRate;
}

void LowFrequencyOscillator::setVolume (float newVolume)
{
    outputVolume.setValue (newVolume);
}

void LowFrequencyOscillator::setType (TypeLFO newType)
{
    type = newType;
}

void LowFrequencyOscillator::initProcessing (double newSampleRate)
{
    sampleRate = newSampleRate;
    
    setFrequency (frequency);
}

void LowFrequencyOscillator::reset (double initialPhase)
{
    outputVolume.reset (sampleRate, 0.05);
    copyPhase (initialPhase);
}

const float LowFrequencyOscillator::getNextSample()
{
    auto output = 0.f;

    if (type == lfoSine)
    {
        output = -std::cos (static_cast<float> (phase));
    }
    else if (type == lfoTriangle)
    {
        if (phase < double_Pi)
            output = 2.f * static_cast<float> (phase / double_Pi) - 1.f;

        else
            output = -2.f * static_cast<float> (phase / double_Pi) + 3.f;
    }
    else if (type == lfoSawtooth)
    {
        output = static_cast<float> (phase / double_Pi) - 1.f;
    }
    else if (type == lfoSquare)
    {
        output = (phase < double_Pi) ? 1.f : -1.f;
    }
    else
    {
        output = theRandom.nextFloat() * 2.f - 1.f;
    }

    phase += factor;
    if (phase >= 2 * double_Pi) phase -= 2 * double_Pi;
    
    return output * outputVolume.getNextValue();
}

void LowFrequencyOscillator::copyPhase (double newPhase)
{
    phase = jlimit (0.0, 2 * double_Pi, newPhase);
}

const double LowFrequencyOscillator::getPhase()
{
    return phase;
}
