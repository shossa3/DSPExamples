/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionSixDSP.h"

//==============================================================================
EnvelopeFollower::EnvelopeFollower()
{
    prmGain.store (0.f);
    prmOutput.store (0.f);

    prmAttack.store (1.f);
    prmRelease.store (50.f);
    
    isActive = false;
}

EnvelopeFollower::~EnvelopeFollower()
{

}

//==============================================================================
void EnvelopeFollower::setPreGain (float gaindB)
{
    prmGain.store (jlimit (-200.f, 100.f, gaindB));
    mustUpdateProcessing = true;
}

void EnvelopeFollower::setVolume (float volumedB)
{
    prmOutput.store (jlimit (-200.f, 100.f, volumedB));
    mustUpdateProcessing = true;
}

void EnvelopeFollower::setAttackTime (float attackTimeMs)
{
    prmAttack.store (jlimit (0.001f, 1000.f, attackTimeMs));
    mustUpdateProcessing = true;
}

void EnvelopeFollower::setReleaseTime (float releaseTimeMs)
{
    prmRelease.store (jlimit (10.f, 50000.f, releaseTimeMs));
    mustUpdateProcessing = true;
}

//==============================================================================
void EnvelopeFollower::initProcessing (double sR)
{
    jassert (sR > 0);
    sampleRate = sR;

    theBallisticsFilter.initProcessing (sR);
    
    updateProcessing();
    reset();

    isActive = true;
}

void EnvelopeFollower::reset (float zeroValue)
{
    gainVolume.reset (sampleRate, 0.05);
    outputVolume.reset (sampleRate, 0.05);

    theBallisticsFilter.reset (zeroValue);
}

float EnvelopeFollower::processSingleSample (float sample)
{
    if (mustUpdateProcessing)
        updateProcessing();

    auto output = sample * gainVolume.getNextValue();

    output = std::abs (output);
    output = theBallisticsFilter.processSingleSample (output);
    
    output = jlimit (0.f, 1.f, output * outputVolume.getNextValue());

    return output;
}

void EnvelopeFollower::updateProcessing()
{
    mustUpdateProcessing = false;

    theBallisticsFilter.setAttackTime (prmAttack.load());
    theBallisticsFilter.setReleaseTime (prmRelease.load());

    gainVolume.setValue (Decibels::decibelsToGain (prmGain.load(), -200.f));
    outputVolume.setValue (Decibels::decibelsToGain (prmOutput.load(), -200.f));
}
