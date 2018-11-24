/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionFourDSP.h"

//==============================================================================
BallisticsFilter::BallisticsFilter()
{
    prmAttack.store (1.f);
    prmRelease.store (100.f);

    isActive = false;
}

BallisticsFilter::~BallisticsFilter()
{

}

void BallisticsFilter::setAttackTime (float attackTimeMs)
{
    prmAttack.store (jlimit (0.001f, 10000.f, attackTimeMs));
    mustUpdateProcessing = true;
}

void BallisticsFilter::setReleaseTime (float releaseTimeMs)
{
    prmRelease.store (jlimit (10.f, 500000.f, releaseTimeMs));
    mustUpdateProcessing = true;
}

void BallisticsFilter::initProcessing (double sR)
{
    sampleRate = sR;

    updateProcessing();
    reset();

    isActive = true;
}

void BallisticsFilter::reset (float zeroValue)
{
    yold = zeroValue;
}

float BallisticsFilter::processSingleSample (float &sample)
{
    if (mustUpdateProcessing)
        updateProcessing();

    auto timeCte = (sample > yold ? cteAT : cteRL);
    auto result = sample + timeCte * (yold - sample);

    yold = result;
    return result;
}

void BallisticsFilter::updateProcessing()
{
    mustUpdateProcessing = false;
    auto Ts = 1.f / (float)sampleRate;

    cteAT = std::exp (-2.f * float_Pi * Ts * 1000.f / prmAttack.load());
    cteRL = std::exp (-2.f * float_Pi * Ts * 1000.f / prmRelease.load());
}
