/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionSixComponent.h"
#include "../Session1/SessionOneDSP.h"
#include "../Session2/SessionTwoDSP.h"
#include "../Session3/SessionThreeDSP.h"
#include "../../Assets/MathFunctions.h"


//==============================================================================
SessionSixComponent::SessionSixComponent (AudioDeviceManager *adm) : SessionComponent ("Filters", adm)
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

    addAndMakeVisible (sliderModulation = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderModulation->setRange (0.0, 100.0);
    sliderModulation->setDoubleClickReturnValue (true, 50.0);
    sliderModulation->setValue (50.0, dontSendNotification);
    sliderModulation->addListener (this);

    addAndMakeVisible (sliderLFOFrequency = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderLFOFrequency->setRange (0.01, 20.0);
    sliderLFOFrequency->setDoubleClickReturnValue (true, 1.0);
    sliderLFOFrequency->setSkewFactor (0.1);
    sliderLFOFrequency->setValue (1.0, dontSendNotification);
    sliderLFOFrequency->addListener (this);

    addAndMakeVisible (sliderEnvAttack = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderEnvAttack->setRange (0.1, 1000.0);
    sliderEnvAttack->setSkewFactor (0.2);
    sliderEnvAttack->setValue (0.1, dontSendNotification);
    sliderEnvAttack->addListener (this);

    addAndMakeVisible (sliderEnvRelease = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderEnvRelease->setRange (10.0, 10000.0);
    sliderEnvRelease->setSkewFactor (0.2);
    sliderEnvRelease->setValue (50.0, dontSendNotification);
    sliderEnvRelease->addListener (this);

    addAndMakeVisible (sliderEnvSens = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderEnvSens->setRange (0.0, 100.0);
    sliderEnvSens->setDoubleClickReturnValue (true, 50.0);
    sliderEnvSens->setValue (50.0, dontSendNotification);
    sliderEnvSens->addListener (this);
        
    // -------------------------------------------------------------------------
    addAndMakeVisible (comboAlgorithm = new ComboBox());
    
    comboAlgorithm->addSectionHeading ("Transposed Direct Form 2");
    comboAlgorithm->addItem ("TDF2", 1);
    comboAlgorithm->addItem ("TDF2 + smoothing", 2);
    
    comboAlgorithm->addSectionHeading ("Topology preserving transform");
    comboAlgorithm->addItem ("TPT", 3);
    comboAlgorithm->addItem ("TPT + smoothing", 4);
    
    comboAlgorithm->addListener (this);
    comboAlgorithm->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (comboModulationType = new ComboBox());
    comboModulationType->addItemList ({ "None", "LFO Triangle", "LFO Square", "Env. Up", "Env. Down" }, 1);
    
    comboModulationType->addListener (this);
    comboModulationType->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (labelFrequency = new Label ("", "Frequency"));
    labelFrequency->setJustificationType (Justification::centred);
    labelFrequency->attachToComponent (sliderFrequency, false);

    addAndMakeVisible (labelResonance = new Label ("", "Resonance"));
    labelResonance->setJustificationType (Justification::centred);
    labelResonance->attachToComponent (sliderResonance, false);

    addAndMakeVisible (labelModulation = new Label ("", "Depth"));
    labelModulation->setJustificationType (Justification::centred);
    labelModulation->attachToComponent (sliderModulation, false);

    addAndMakeVisible (labelLFOFrequency = new Label ("", "LFO freq."));
    labelLFOFrequency->setJustificationType (Justification::centred);
    labelLFOFrequency->attachToComponent (sliderLFOFrequency, false);

    addAndMakeVisible (labelEnvAttack = new Label ("", "Attack"));
    labelEnvAttack->setJustificationType (Justification::centred);
    labelEnvAttack->attachToComponent (sliderEnvAttack, false);

    addAndMakeVisible (labelEnvRelease = new Label ("", "Release"));
    labelEnvRelease->setJustificationType (Justification::centred);
    labelEnvRelease->attachToComponent (sliderEnvRelease, false);

    addAndMakeVisible (labelEnvSens = new Label ("", "Sensitivity"));
    labelEnvSens->setJustificationType (Justification::centred);
    labelEnvSens->attachToComponent (sliderEnvSens, false);
        
    addAndMakeVisible (labelAlgorithm = new Label ("", "Structure"));
    labelAlgorithm->setJustificationType (Justification::centred);
    labelAlgorithm->attachToComponent (comboAlgorithm, false);

    addAndMakeVisible (labelModulationType = new Label ("", "Modulation Type"));
    labelModulationType->setJustificationType (Justification::centred);
        
    // -------------------------------------------------------------------------
    lastEnvValue.store (0.5f);

    // -------------------------------------------------------------------------
    updateVisibility();
    triggerAsyncUpdate();
}

SessionSixComponent::~SessionSixComponent()
{
    
}

void SessionSixComponent::paint (Graphics &g)
{
    auto rect = getLocalBounds();

    g.setColour (Colours::white);
    g.fillRect (rect.removeFromTop (getHeight() - 170));
    
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (rect);
}

void SessionSixComponent::resized()
{
    Rectangle<int> rect = getLocalBounds();
    
    auto rectTop = rect.removeFromTop (getHeight() - 170);
    thePlotComponent->setBounds (rectTop.reduced (10, 10));

    rect.removeFromTop (40);
    auto rectMid = rect.removeFromTop (100).reduced (10, 0);

    comboAlgorithm->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (0, -28));
    comboModulationType->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (0, 28));
    labelModulationType->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24).translated (0, 52));

    sliderFrequency->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200, 0));
    sliderResonance->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 100, 0));
    
    sliderModulation->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 200, 0));
    sliderLFOFrequency->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 300, 0));

    sliderEnvAttack->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 200, 0));
    sliderEnvRelease->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 300, 0));
    sliderEnvSens->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 400, 0));
}

//==============================================================================
void SessionSixComponent::sliderValueChanged (Slider *slider)
{
    if (slider == sliderFrequency || slider == sliderResonance)
    {
        mustUpdateProcessing = true;
    }
    else if (slider == sliderModulation || slider == sliderLFOFrequency)
    {
        mustUpdateProcessing = true;
    }
    else if (slider == sliderEnvAttack || slider == sliderEnvRelease || slider == sliderEnvSens)
    {
        mustUpdateProcessing = true;
    }
}

void SessionSixComponent::comboBoxChanged (ComboBox *combo)
{
    if (combo == comboAlgorithm)
    {
        mustUpdateProcessing = true;
    }
    else if (combo == comboModulationType)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
}

void SessionSixComponent::updateVisibility()
{
    auto modulation = comboModulationType->getSelectedId() - 1;

    labelModulation->setVisible (modulation == Modulation_LFO_Square || modulation == Modulation_LFO_Triangle);
    labelLFOFrequency->setVisible (modulation == Modulation_LFO_Square || modulation == Modulation_LFO_Triangle);
    sliderModulation->setVisible (modulation == Modulation_LFO_Square || modulation == Modulation_LFO_Triangle);
    sliderLFOFrequency->setVisible (modulation == Modulation_LFO_Square || modulation == Modulation_LFO_Triangle);

    labelEnvAttack->setVisible (modulation == Modulation_Envelope_Up || modulation == Modulation_Envelope_Down);
    labelEnvRelease->setVisible (modulation == Modulation_Envelope_Up || modulation == Modulation_Envelope_Down);
    labelEnvSens->setVisible (modulation == Modulation_Envelope_Up || modulation == Modulation_Envelope_Down);
    sliderEnvAttack->setVisible (modulation == Modulation_Envelope_Up || modulation == Modulation_Envelope_Down);
    sliderEnvRelease->setVisible (modulation == Modulation_Envelope_Up || modulation == Modulation_Envelope_Down);
    sliderEnvSens->setVisible (modulation == Modulation_Envelope_Up || modulation == Modulation_Envelope_Down);
}

//==============================================================================
void SessionSixComponent::prepareToPlay (int samplesPerBlockExpected, double sR)
{
    SessionComponent::prepareToPlay (samplesPerBlockExpected, sR);

    bufferModulation.setSize (1, samplesPerBlockExpected);
    theLFO.initProcessing (sR);
    theEnvelopeFollower.initProcessing (sR);
    theSlewFilter.initProcessing (sR);

    updateProcessing();
    reset();

    isActive = true;
}

void SessionSixComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (mustUpdateProcessing)
        updateProcessing();

    ScopedNoDenormals noDenormals;

    auto startSample = bufferToFill.startSample;
    auto numSamples = bufferToFill.numSamples;
    auto numChannels = jmax (2, bufferToFill.buffer->getNumChannels());

    jassert (numSamples > 0);
    jassert (numChannels > 0);

    auto *envSamples = bufferModulation.getWritePointer (0);

    for (auto i = 0; i < numSamples; i++)
        envSamples[i] = centralFreq;

    if (modulationType == Modulation_LFO_Square || modulationType == Modulation_LFO_Triangle)
    {
        for (auto i = 0; i < numSamples; i++)
            envSamples[i] += theLFO.getNextSample() * depthVolume.getNextValue();
    }
    else if (modulationType == Modulation_Envelope_Up || modulationType == Modulation_Envelope_Down)
    {
        for (auto i = 0; i < numSamples; i++)
        {
            auto input = 0.f;

            for (auto channel = 0; channel < numChannels; channel++)
                input += bufferToFill.buffer->getSample (channel, startSample + i);

            input /= (float)numChannels;
            
            envSamples[i] += theEnvelopeFollower.processSingleSample (input)
                * (modulationType == Modulation_Envelope_Down ? -1.f : 1.f);
        }
    }
    
    if (isSmoothing)
    {
        for (auto i = 0; i < numSamples; i++)
            envSamples[i] = theSlewFilter.processSingleSampleRaw (envSamples[i]);
    }

    for (auto i = 0; i < numSamples; i++)
        envSamples[i] = jlimit (0.f, 1.f, envSamples[i]);

    lastEnvValue.store (envSamples[numSamples - 1]);

    if (algorithmType == Structure_TDF2 || algorithmType == Structure_TDF2_Smoothing)
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            float b0, b1, b2, a1, a2;

            auto *samples = bufferToFill.buffer->getWritePointer (channel);
            
            auto v1 = theFiltersTDF2[channel].v1;
            auto v2 = theFiltersTDF2[channel].v2;

            for (auto i = 0; i < numSamples; i++)
            {
                auto input = samples[i + startSample];
                auto frequency = jmaplintolog (envSamples[i], 20.f, 22000.f);

                SessionOneDSP::calculusCoefficients (frequency, (float)sliderResonance->getValue(), 0.f,
                    SessionOneDSP::Type_LowPass2, sampleRate, b0, b1, b2, a1, a2);

                auto output = b0 * input + v1;
                v1 = b1 * input + v2 - a1 * output;
                v2 = b2 * input - a2 * output;

                samples[i + startSample] = output;
            }

            theFiltersTDF2[channel].v1 = v1;
            theFiltersTDF2[channel].v2 = v2;
        }
    }
    else
    {
        for (auto channel = 0; channel < numChannels; channel++)
        {
            float g, h;

            auto *samples = bufferToFill.buffer->getWritePointer (channel);
            
            auto s1 = theFiltersTPT[channel].s1;
            auto s2 = theFiltersTPT[channel].s2;

            for (auto i = 0; i < numSamples; i++)
            {
                auto input = samples[i + startSample];
                auto frequency = jmaplintolog (envSamples[i], 20.f, 22000.f);

                g = std::tan (float_Pi * frequency / (float)sampleRate);
                h = 1.f / (1 + R2 * g + g * g);

                auto yH = (input - s1 * (R2 + g) - s2) * h;
                auto yB = g * yH + s1;
                auto yL = g * yB + s2;

                s1 = g * yH + yB;
                s2 = g * yB + yL;

                samples[i + startSample] = yL;
            }

            theFiltersTPT[channel].s1 = s1;
            theFiltersTPT[channel].s2 = s2;
        }
    }
}

void SessionSixComponent::releaseAudioResources()
{
    reset();
}

void SessionSixComponent::reset()
{
    for (auto channel = 0; channel < 2; channel++)
    {
        theFiltersTDF2[channel].v1 = 0;
        theFiltersTDF2[channel].v2 = 0;

        theFiltersTPT[channel].s1 = 0;
        theFiltersTPT[channel].s2 = 0;
    }

    theLFO.reset();
    depthVolume.reset (sampleRate, 0.05);
    theEnvelopeFollower.reset();
    theSlewFilter.reset();
}

void SessionSixComponent::updateProcessing()
{
    mustUpdateProcessing = false;

    algorithmType = comboAlgorithm->getSelectedId() - 1;
    modulationType = comboModulationType->getSelectedId() - 1;

    centralFreq = (float)jmaplogtolin (sliderFrequency->getValue(), 20.0, 22000.0);
    R2 = 1.f / (float) sliderResonance->getValue();

    if (modulationType == Modulation_LFO_Square || modulationType == Modulation_LFO_Triangle)
    {
        theLFO.setType (modulationType == Modulation_LFO_Triangle ? 
            LowFrequencyOscillator::lfoTriangle : LowFrequencyOscillator::lfoSquare);
        theLFO.setFrequency ((float) sliderLFOFrequency->getValue());
        depthVolume.setValue ((float)sliderModulation->getValue() / 100.f);
    }
    else if (modulationType == Modulation_Envelope_Up || modulationType == Modulation_Envelope_Down)
    {
        theEnvelopeFollower.setAttackTime ((float)sliderEnvAttack->getValue());
        theEnvelopeFollower.setReleaseTime ((float)sliderEnvRelease->getValue());
        theEnvelopeFollower.setPreGain ((float)sliderEnvSens->getValue() - 50.0f);
    }

    theSlewFilter.setType (TPTFilter1stOrder::TypeLowPass);
    theSlewFilter.setCutoffFrequency (20.f);

    isSmoothing = (algorithmType == Structure_TDF2_Smoothing || algorithmType == Structure_TPT_Smoothing);
}

//==============================================================================
void SessionSixComponent::timerCallback()
{
    stopThread (1000);
    startThread();
}

void SessionSixComponent::run()
{
    if (!(isActive))
        return;
    
    // -------------------------------------------------------------------------
    auto N = 1001;
    bufferData.setSize (2, N, false, false, true);

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    float b0D, b1D, b2D, a1D, a2D;

    auto cutoffFrequency = jmaplintolog (jlimit (0.f, 1.f, lastEnvValue.load()), 20.f, 22000.f);
    
    SessionOneDSP::calculusCoefficients (cutoffFrequency, (float)sliderResonance->getValue(), 0.f,
        SessionOneDSP::Type_LowPass2, sampleRate,
        b0D, b1D, b2D, a1D, a2D);

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    auto *dataX = bufferData.getWritePointer (0);
    auto *dataYD = bufferData.getWritePointer (1);
        
    for (auto i = 0; i < N; i++)
    {
        auto frequency = jmaplintolog (i / (N - 1.0), 10.0, 22000.0);
        auto magnitudeD = SessionOneDSP::getMagnitudeForFrequency (frequency,
            sampleRate, b0D, b1D, b2D, a1D, a2D);
        
        dataX[i] = (float)frequency;
        dataYD[i] = (float)Decibels::gainToDecibels (magnitudeD);

        if (threadShouldExit())
            return;
    }  

    // -------------------------------------------------------------------------
    PlotComponent::Information information;

    information.minimumXValue = 10.0;
    information.maximumXValue = 22000.0;
    information.isXAxisLogarithmic = true;

    information.minimumYValue = -80.0;
    information.maximumYValue = 40.0;
    information.gridYSize = 10.0;
    
    information.displayGrid = true;
    information.strLabelX = "frequency (Hz)";
    information.strLabelY = "magnitude (dB)";
    
    thePlotComponent->setInformation (information);

    // -------------------------------------------------------------------------
    thePlotComponent->pushData (bufferData, N, 1);
    
    triggerAsyncUpdate();
}
