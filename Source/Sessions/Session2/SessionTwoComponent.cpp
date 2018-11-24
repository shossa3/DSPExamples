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

#include "SessionTwoComponent.h"
#include "../Session1/SessionOneDSP.h"
#include "SessionTwoDSP.h"
#include "../../Assets/MathFunctions.h"

//==============================================================================
SessionTwoComponent::SessionTwoComponent (AudioDeviceManager *adm) : SessionComponent ("Frequency warping", adm)
{
    // -------------------------------------------------------------------------
    addAndMakeVisible (thePlotComponent = new PlotComponent());
            
    // -------------------------------------------------------------------------
    addAndMakeVisible (sliderFrequency = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderFrequency->setRange (20.0, 22000.0);
    sliderFrequency->setSkewFactor (0.2);
    sliderFrequency->setValue (1000.0, dontSendNotification);
    sliderFrequency->addListener (this);

    addAndMakeVisible (sliderResonance = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderResonance->setRange (0.01, 100.0);
    sliderResonance->setDoubleClickReturnValue (true, 1.0 / std::sqrt (2.0));
    sliderResonance->setSkewFactor (0.1);
    sliderResonance->setValue (1.0 / std::sqrt (2.0), dontSendNotification);
    sliderResonance->addListener (this);

    addAndMakeVisible (sliderGain = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderGain->setRange (-40.0, 40.0);
    sliderGain->setDoubleClickReturnValue (true, 0.0);
    sliderGain->addListener (this);

    // -------------------------------------------------------------------------
    addAndMakeVisible (comboType = new ComboBox());
    comboType->addItemList ({ "Low-pass 1st order", "High-pass 1st order",
        "Low-pass 2nd order", "Band-pass 2nd order", "High-pass 2nd order",
        "Peak filter"}, 1);

    comboType->addListener (this);
    comboType->setSelectedId (1, dontSendNotification);

    addAndMakeVisible (comboDisplay = new ComboBox());
    comboDisplay->addItemList ({ "Magnitude (A+D)", "Phase (A+D)" }, 1);
    comboDisplay->addListener (this);
    comboDisplay->setSelectedId (1, dontSendNotification);

    addAndMakeVisible (comboAlgorithm = new ComboBox());
    comboAlgorithm->addItemList ({ "BLT - no warping", "BLT - warping" }, 1);
    comboAlgorithm->addListener (this);
    comboAlgorithm->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (labelFrequency = new Label ("", "Frequency"));
    labelFrequency->setJustificationType (Justification::centred);
    labelFrequency->attachToComponent (sliderFrequency, false);

    addAndMakeVisible (labelResonance = new Label ("", "Resonance"));
    labelResonance->setJustificationType (Justification::centred);
    labelResonance->attachToComponent (sliderResonance, false);

    addAndMakeVisible (labelGain = new Label ("", "Gain"));
    labelGain->setJustificationType (Justification::centred);
    labelGain->attachToComponent (sliderGain, false);

    addAndMakeVisible (labelType = new Label ("", "Type"));
    labelType->setJustificationType (Justification::centred);
    labelType->attachToComponent (comboType, false);

    addAndMakeVisible (labelDisplay = new Label ("", "Display"));
    labelDisplay->setJustificationType (Justification::centred);
    labelDisplay->attachToComponent (comboDisplay, false);

    addAndMakeVisible (labelAlgorithm = new Label ("", "Algorithm"));
    labelAlgorithm->setJustificationType (Justification::centred);
    
    // -------------------------------------------------------------------------
    updateVisibility();
    triggerAsyncUpdate();
}

SessionTwoComponent::~SessionTwoComponent()
{
    
}

void SessionTwoComponent::paint (Graphics &g)
{
    auto rect = getLocalBounds();

    g.setColour (Colours::white);
    g.fillRect (rect.removeFromTop (getHeight() - 170));
    
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (rect);
}

void SessionTwoComponent::resized()
{
    Rectangle<int> rect = getLocalBounds();
    
    auto rectTop = rect.removeFromTop (getHeight() - 170);
    thePlotComponent->setBounds (rectTop.reduced (10, 10));

    rect.removeFromTop (40);
    auto rectMid = rect.removeFromTop (100).reduced (10, 0);

    comboType->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (0, -28));
    comboAlgorithm->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (0, 28));
    labelAlgorithm->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (0, 52));

    sliderFrequency->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200, 0));
    sliderResonance->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 100, 0));
    sliderGain->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 200, 0));

    comboDisplay->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (200 + 320, 0));
}

//==============================================================================
void SessionTwoComponent::sliderValueChanged (Slider *slider)
{
    if (slider == sliderFrequency)
    {
        mustUpdateProcessing = true;
    }

    else if (slider == sliderResonance)
    {
        mustUpdateProcessing = true;
    }

    else if (slider == sliderGain)
    {
        mustUpdateProcessing = true;
    }
}

void SessionTwoComponent::comboBoxChanged (ComboBox *combo)
{
    if (combo == comboType)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
    else if (combo == comboDisplay)
    {

    }
    else if (combo == comboAlgorithm)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
}

void SessionTwoComponent::updateVisibility()
{
    auto type = comboType->getSelectedId() - 1;

    labelResonance->setEnabled (type != SessionTwoDSP::Type_LowPass1 && type != SessionTwoDSP::Type_HighPass1);
    sliderResonance->setEnabled (type != SessionTwoDSP::Type_LowPass1 && type != SessionTwoDSP::Type_HighPass1);

    labelGain->setEnabled (type == SessionTwoDSP::Type_Peak);
    sliderGain->setEnabled (type == SessionTwoDSP::Type_Peak);
}

//==============================================================================
void SessionTwoComponent::prepareToPlay (int samplesPerBlockExpected, double sR)
{
    SessionComponent::prepareToPlay (samplesPerBlockExpected, sR);

    updateProcessing();
    reset();

    isActive = true;
}

void SessionTwoComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (mustUpdateProcessing)
        updateProcessing();

    ScopedNoDenormals noDenormals;

    auto startSample = bufferToFill.startSample;
    auto numSamples = bufferToFill.numSamples;
    auto numChannels = jmax (2, bufferToFill.buffer->getNumChannels());

    jassert (numSamples > 0);
    jassert (numChannels > 0);

    for (auto channel = 0; channel < numChannels; channel++)
    {
        auto *samples = bufferToFill.buffer->getWritePointer (channel);
        
        auto b0 = theCoefficients.b0;
        auto b1 = theCoefficients.b1;
        auto b2 = theCoefficients.b2;
        auto a1 = theCoefficients.a1;
        auto a2 = theCoefficients.a2;
        
        auto v1 = theFilters[channel].v1;
        auto v2 = theFilters[channel].v2;

        for (auto i = 0; i < numSamples; i++)
        {
            auto input = samples[i + startSample];
                        
            auto output = b0 * input + v1;
            v1 = b1 * input + v2 - a1 * output;
            v2 = b2 * input - a2 * output;

            samples[i + startSample] = output;
        }

        theFilters[channel].v1 = v1;
        theFilters[channel].v2 = v2;
    }
}

void SessionTwoComponent::releaseAudioResources()
{
    reset();
}

void SessionTwoComponent::reset()
{
    for (auto channel = 0; channel < 2; channel++)
    {
        theFilters[channel].v1 = 0;
        theFilters[channel].v2 = 0;
    }
}

void SessionTwoComponent::updateProcessing()
{
    mustUpdateProcessing = false;
    auto mustWarpFrequency = comboAlgorithm->getSelectedId() == 2;

    if (mustWarpFrequency)
    {
        SessionTwoDSP::calculusPreWarpedCoefficients ((float)sliderFrequency->getValue(), (float)sliderResonance->getValue(),
            (float)sliderGain->getValue(),
            comboType->getSelectedId() - 1, sampleRate,
            theCoefficients.b0, theCoefficients.b1, theCoefficients.b2,
            theCoefficients.a1, theCoefficients.a2);
    }
    else
    {
        SessionOneDSP::calculusCoefficients ((float)sliderFrequency->getValue(), (float)sliderResonance->getValue(),
            (float)sliderGain->getValue(),
            comboType->getSelectedId() - 1, sampleRate,
            theCoefficients.b0, theCoefficients.b1, theCoefficients.b2,
            theCoefficients.a1, theCoefficients.a2);
    }
}

//==============================================================================
void SessionTwoComponent::timerCallback()
{
    stopThread (1000);
    startThread();
}

void SessionTwoComponent::run()
{
    if (!(isActive))
        return;
    
    // -------------------------------------------------------------------------
    auto N = 1001;
    bufferData.setSize (3, N, false, false, true);

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    float b0A, b1A, b2A, a1A, a2A;
    float b0D, b1D, b2D, a1D, a2D;
    auto mustWarpFrequency = comboAlgorithm->getSelectedId() == 2;

    SessionTwoDSP::calculusAnalogCoefficients ((float)sliderFrequency->getValue(),
        (float)sliderResonance->getValue(),
        (float)sliderGain->getValue(),
        comboType->getSelectedId() - 1,
        b0A, b1A, b2A, a1A, a2A);

    if (mustWarpFrequency)
    {
        SessionTwoDSP::calculusPreWarpedCoefficients ((float)sliderFrequency->getValue(),
            (float)sliderResonance->getValue(),
            (float)sliderGain->getValue(),
            comboType->getSelectedId() - 1, sampleRate,
            b0D, b1D, b2D, a1D, a2D);
    }
    else
    {
        SessionOneDSP::calculusCoefficients ((float)sliderFrequency->getValue(),
            (float)sliderResonance->getValue(),
            (float)sliderGain->getValue(),
            comboType->getSelectedId() - 1, sampleRate,
            b0D, b1D, b2D, a1D, a2D);
    }

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    auto *dataX = bufferData.getWritePointer (0);
    auto *dataYA = bufferData.getWritePointer (1);
    auto *dataYD = bufferData.getWritePointer (2);

    auto displayMagnitude = comboDisplay->getSelectedId() == 1;

    if (displayMagnitude)
    {
        for (auto i = 0; i < N; i++)
        {
            auto frequency = jmaplintolog (i / (N - 1.0), 10.0, 22000.0);
            auto magnitudeA = SessionTwoDSP::getAnalogMagnitudeForFrequency (frequency,
                b0A, b1A, b2A, a1A, a2A);
            auto magnitudeD = SessionOneDSP::getMagnitudeForFrequency (frequency, sampleRate,
                b0D, b1D, b2D, a1D, a2D);

            dataX[i] = (float)frequency;
            dataYA[i] = (float)Decibels::gainToDecibels (magnitudeA);
            dataYD[i] = (float)Decibels::gainToDecibels (magnitudeD);

            if (threadShouldExit())
                return;
        }
    }
    else
    {
        for (auto i = 0; i < N; i++)
        {
            auto frequency = jmaplintolog (i / (N - 1.0), 10.0, 22000.0);
            auto phaseA = SessionTwoDSP::getAnalogPhaseForFrequency (frequency,
                b0A, b1A, b2A, a1A, a2A);
            auto phaseD = SessionOneDSP::getPhaseForFrequency (frequency, sampleRate,
                b0D, b1D, b2D, a1D, a2D);

            dataX[i] = (float)frequency;
            dataYA[i] = (float)(phaseA / double_Pi * 180.0);
            dataYD[i] = (float)(phaseD / double_Pi * 180.0);

            if (threadShouldExit())
                return;
        }
    }    

    // -------------------------------------------------------------------------
    PlotComponent::Information information;

    information.minimumXValue = 10.0;
    information.maximumXValue = 22000.0;
    information.isXAxisLogarithmic = true;

    if (displayMagnitude)
    {
        information.minimumYValue = -80.0;
        information.maximumYValue = 40.0;
        information.gridYSize = 10.0;
    }
    else
    {
        information.minimumYValue = -200.0;
        information.maximumYValue = 200.0;
        information.gridYSize = 45.0;
        information.gridYMustStartAtZero = true;
    }    

    information.displayGrid = true;
    information.strLabelX = "frequency (Hz)";
    
    if (displayMagnitude)
        information.strLabelY = "magnitude (dB)";
    else
        information.strLabelY = "phase (degrees)";

    thePlotComponent->setInformation (information);

    // -------------------------------------------------------------------------
    thePlotComponent->pushData (bufferData, N, 2);
    
    triggerAsyncUpdate();
}
