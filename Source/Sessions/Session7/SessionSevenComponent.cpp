/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionSevenComponent.h"
#include "../Session1/SessionOneDSP.h"
#include "../Session2/SessionTwoDSP.h"
#include "../Session3/SessionThreeDSP.h"
#include "../../Assets/MathFunctions.h"


//==============================================================================
SessionSevenComponent::SessionSevenComponent (AudioDeviceManager *adm) : SessionComponent ("Models", adm)
{
    // -------------------------------------------------------------------------
    addAndMakeVisible (thePlotComponent = new PlotComponent());
            
    // -------------------------------------------------------------------------
    addAndMakeVisible (sliderGain = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderGain->setRange (-40.0, 40.0);
    sliderGain->setDoubleClickReturnValue (true, 0.0);
    sliderGain->setValue (0.0, dontSendNotification);
    sliderGain->addListener (this);
    
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
    addAndMakeVisible (comboModel = new ComboBox());
    comboModel->addItem ("SVF NL (naive)", 1);
    comboModel->addItem ("Moog filter (JUCE)", 2);
    comboModel->addItem ("Diode Clipper (NR)", 3);
    comboModel->addListener (this);
    comboModel->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (comboModulationType = new ComboBox());
    comboModulationType->addItemList ({ "None", "LFO Triangle", "LFO Square", "Env. Up", "Env. Down" }, 1);
    
    comboModulationType->addListener (this);
    comboModulationType->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (labelGain = new Label ("", "Drive"));
    labelGain->setJustificationType (Justification::centred);
    labelGain->attachToComponent (sliderGain, false);
    
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
        
    addAndMakeVisible (labelModel = new Label ("", "Model"));
    labelModel->setJustificationType (Justification::centred);
    labelModel->attachToComponent (comboModel, false);

    addAndMakeVisible (labelModulationType = new Label ("", "Modulation Type"));
    labelModulationType->setJustificationType (Justification::centred);
        
    // -------------------------------------------------------------------------
    lastEnvValue.store (0.5f);

    // -------------------------------------------------------------------------
    theOversampling.reset (new dsp::Oversampling<float> (2));

    theOversampling->clearOversamplingStages();
    theOversampling->addOversamplingStage (dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, 0.05f, -90.f, 0.05f, -90.f);
    theOversampling->addOversamplingStage (dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, 0.05f, -90.f, 0.05f, -90.f);

    // -------------------------------------------------------------------------
    updateVisibility();
    triggerAsyncUpdate();
}

SessionSevenComponent::~SessionSevenComponent()
{
    
}

void SessionSevenComponent::paint (Graphics &g)
{
    auto rect = getLocalBounds();

    g.setColour (Colours::white);
    g.fillRect (rect.removeFromTop (getHeight() - 170));
    
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (rect);
}

void SessionSevenComponent::resized()
{
    Rectangle<int> rect = getLocalBounds();
    
    auto rectTop = rect.removeFromTop (getHeight() - 170);
    thePlotComponent->setBounds (rectTop.reduced (10, 10));

    rect.removeFromTop (40);
    auto rectMid = rect.removeFromTop (100).reduced (10, 0);

    comboModel->setBounds (rectMid.withWidth (130).withSizeKeepingCentre (130, 24).translated (0, -28));
    comboModulationType->setBounds (rectMid.withWidth (130).withSizeKeepingCentre (130, 24).translated (0, 28));
    labelModulationType->setBounds (rectMid.withWidth (130).withSizeKeepingCentre (130, 24).translated (0, 52));

    auto sliderSize = 70;
    auto i = 0;

    rectMid = rectMid.withWidth (sliderSize);

    sliderGain->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    sliderFrequency->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    sliderResonance->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    
    sliderModulation->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    sliderLFOFrequency->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    i -= 2;

    sliderEnvAttack->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    sliderEnvRelease->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
    sliderEnvSens->setBounds (rectMid.withSizeKeepingCentre (sliderSize, sliderSize).translated (160 + (sliderSize + 20) * i, 0)); i++;
}

//==============================================================================
void SessionSevenComponent::sliderValueChanged (Slider *slider)
{
    if (slider == sliderGain || slider == sliderResonance)
    {
        mustUpdateProcessing = true;
    }
    else if (slider == sliderFrequency)
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

void SessionSevenComponent::comboBoxChanged (ComboBox *combo)
{
    if (combo == comboModel)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
    else if (combo == comboModulationType)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
}

void SessionSevenComponent::updateVisibility()
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

    sliderResonance->setEnabled (comboModel->getSelectedId() - 1 != Model_DiodeClipper_NewtonRaphson);
}

//==============================================================================
void SessionSevenComponent::prepareToPlay (int samplesPerBlockExpected, double sR)
{
    SessionComponent::prepareToPlay (samplesPerBlockExpected, sR);

    auto oversamplingFactor = theOversampling->getOversamplingFactor();
    theOversampling->initProcessing (samplesPerBlockExpected);

    for (auto channel = 0; channel < 2; channel++)
    {
        theFiltersNL[channel].initProcessing (sR * oversamplingFactor);
        theDiodeClipper[channel].initProcessing (sR * oversamplingFactor);
    }
    
    theMoogFilter.prepare ({ sR * oversamplingFactor, (uint32) (samplesPerBlockExpected * oversamplingFactor), 2u });
    theMoogFilter.setMode (dsp::LadderFilter<float>::Mode::LPF24);
    
    bufferModulation.setSize (1, samplesPerBlockExpected * (int) oversamplingFactor);

    theLFO.initProcessing (sR * oversamplingFactor);
    theEnvelopeFollower.initProcessing (sR * oversamplingFactor);
    theSlewFilter.initProcessing (sR * oversamplingFactor);

    updateProcessing();
    reset();

    isActive = true;
}

void SessionSevenComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (mustUpdateProcessing)
        updateProcessing();

    ScopedNoDenormals noDenormals;

    auto startSample = bufferToFill.startSample;
    auto numSamples = bufferToFill.numSamples;
    auto numSamplesOversampled = bufferToFill.numSamples * (int)theOversampling->getOversamplingFactor();
    auto numChannels = jmax (2, bufferToFill.buffer->getNumChannels());

    jassert (numSamples > 0);
    jassert (numChannels > 0);

    dsp::AudioBlock<float> inoutBlock (*(bufferToFill.buffer));
    inoutBlock = inoutBlock.getSubBlock (startSample, numSamples);
    
    dsp::AudioBlock<float> oversampledBlock = theOversampling->processSamplesUp (inoutBlock);

    auto *envSamples = bufferModulation.getWritePointer (0);

    for (auto i = 0; i < numSamplesOversampled; i++)
        envSamples[i] = centralFreq;

    if (modulationType == Modulation_LFO_Square || modulationType == Modulation_LFO_Triangle)
    {
        for (auto i = 0; i < numSamplesOversampled; i++)
            envSamples[i] += theLFO.getNextSample() * depthVolume.getNextValue();
    }
    else if (modulationType == Modulation_Envelope_Up || modulationType == Modulation_Envelope_Down)
    {
        for (auto i = 0; i < numSamplesOversampled; i++)
        {
            auto input = 0.f;

            for (auto channel = 0; channel < numChannels; channel++)
                input += oversampledBlock.getSample (channel, i);

            input /= (float)numChannels;

            envSamples[i] += theEnvelopeFollower.processSingleSample (input)
                * (modulationType == Modulation_Envelope_Down ? -1.f : 1.f);
        }
    }

    for (auto i = 0; i < numSamplesOversampled; i++)
        envSamples[i] = theSlewFilter.processSingleSampleRaw (envSamples[i]);
    
    for (auto i = 0; i < numSamplesOversampled; i++)
        envSamples[i] = jlimit (0.f, 1.f, envSamples[i]);
    
    lastEnvValue.store (envSamples[numSamples - 1]);

    if (modelType == Model_SVFNL_Naive)
    {
        oversampledBlock.multiply (gainVolume);
        
        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto *samples = oversampledBlock.getChannelPointer (channel);

            for (auto i = 0; i < numSamplesOversampled; i++)
            {
                auto frequency = jmaplintolog (envSamples[i], 20.f, 22000.f);
                theFiltersNL[channel].setCutoffFrequency (frequency);
                
                samples[i] = theFiltersNL[channel].processSingleSampleRaw (samples[i]);
            }
        }
    }
    else if (modelType == Model_MoogFilter_JUCE)
    {
        for (auto i = 0; i < numSamplesOversampled; i++)
        {
            auto frequency = jmaplintolog (envSamples[i], 20.f, 22000.f);
            theMoogFilter.setCutoffFrequencyHz (frequency);

            auto blockSample = oversampledBlock.getSubBlock (i, 1);
            dsp::ProcessContextReplacing<float> context (blockSample);
            theMoogFilter.process (context);
        }
    }
    else if (modelType == Model_DiodeClipper_NewtonRaphson)
    {
        oversampledBlock.multiply (gainVolume);

        for (auto channel = 0; channel < numChannels; channel++)
        {
            auto *samples = oversampledBlock.getChannelPointer (channel);

            for (auto i = 0; i < numSamplesOversampled; i++)
            {
                auto frequency = jmaplintolog (envSamples[i], 20.f, 22000.f);
                theDiodeClipper[channel].setCutoffFrequency (frequency);

                samples[i] = theDiodeClipper[channel].processSingleSampleRaw (samples[i]);
            }
        }
    }

    theOversampling->processSamplesDown (inoutBlock);
}

void SessionSevenComponent::releaseAudioResources()
{
    reset();
}

void SessionSevenComponent::reset()
{
    for (auto channel = 0; channel < 2; channel++)
    {
        theFiltersNL[channel].reset();
        theDiodeClipper[channel].reset();
    }

    theMoogFilter.reset();
        
    auto oversamplingFactor = theOversampling->getOversamplingFactor();

    theLFO.reset();
    gainVolume.reset (sampleRate * oversamplingFactor, 0.05);
    depthVolume.reset (sampleRate * oversamplingFactor, 0.05);
    theEnvelopeFollower.reset();
    theSlewFilter.reset();

    theOversampling->reset();
}

void SessionSevenComponent::updateProcessing()
{
    mustUpdateProcessing = false;

    gainVolume.setValue (Decibels::decibelsToGain ((float)sliderGain->getValue()));

    modelType = comboModel->getSelectedId() - 1;
    modulationType = comboModulationType->getSelectedId() - 1;

    centralFreq = (float)jmaplogtolin (sliderFrequency->getValue(), 20.0, 22000.0);

    for (auto channel = 0; channel < 2; channel++)
        theFiltersNL[channel].setResonance ((float) sliderResonance->getValue());

    theMoogFilter.setResonance ((float)jmaplogtolin (sliderResonance->getValue(), 0.01, 100.0));
    theMoogFilter.setDrive (jmax (1.f, Decibels::decibelsToGain ((float)sliderGain->getValue())));

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
}

//==============================================================================
void SessionSevenComponent::timerCallback()
{
    stopThread (1000);
    startThread();
}

void SessionSevenComponent::run()
{
    if (!(isActive))
        return;
    
    // -------------------------------------------------------------------------
    auto N = 2001;
    bufferData.setSize (3, N, false, false, true);

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    auto *samples = bufferData.getWritePointer (0);

    for (auto i = 0; i < N; i++)
        samples[i] = i / (float)sampleRate * 1000.f;

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    LowFrequencyOscillator theLFODisplay;

    theLFODisplay.setFrequency (100.0);
    theLFODisplay.setType (LowFrequencyOscillator::lfoSine);
    theLFODisplay.initProcessing (sampleRate);
    theLFODisplay.reset (7.f * float_Pi / 4.f);

    auto dataY = bufferData.getWritePointer (1);

    for (auto i = 0; i < N; i++)
        dataY[i] = theLFODisplay.getNextSample();

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    auto cutoffFrequency = jmaplintolog (jlimit (0.f, 1.f, lastEnvValue.load()), 20.f, 22000.f);
    auto Q = (float)sliderResonance->getValue();
    auto drive = (float)Decibels::decibelsToGain (sliderGain->getValue());

    auto model = comboModel->getSelectedId() - 1;

    if (model == Model_SVFNL_Naive)
    {
        SVFLowpassFilterNL theFilter;
        theFilter.setCutoffFrequency (cutoffFrequency);
        theFilter.setResonance (Q);
        theFilter.initProcessing (sampleRate);

        for (auto i = 0; i < N; i++)
        {
            auto result = drive * dataY[i];
            result = theFilter.processSingleSampleRaw (result);

            dataY[i] = result;
        }
    }
    else if (model == Model_MoogFilter_JUCE)
    {
        dsp::LadderFilter<float> ladderFilter;
        ladderFilter.setMode (dsp::LadderFilter<float>::Mode::LPF24);
        ladderFilter.setDrive (jmax (1.f, (float)Decibels::decibelsToGain (sliderGain->getValue())));
        ladderFilter.setCutoffFrequencyHz (cutoffFrequency);
        ladderFilter.setResonance ((float)jmaplogtolin (sliderResonance->getValue(), 0.01, 100.0));
        ladderFilter.prepare ({ sampleRate, (uint32) N, 1u });

        dsp::AudioBlock<float> inoutBlock (bufferData);
        inoutBlock = inoutBlock.getSingleChannelBlock (1);
        dsp::ProcessContextReplacing<float> context (inoutBlock);

        ladderFilter.process (context);
    }
    else if (model == Model_DiodeClipper_NewtonRaphson)
    {
        DiodeClipperADC18 diodeClipper;
        diodeClipper.setCutoffFrequency (cutoffFrequency);
        diodeClipper.initProcessing (sampleRate);

        for (auto i = 0; i < N; i++)
        {
            auto result = drive * dataY[i];
            result = diodeClipper.processSingleSampleRaw (result);

            dataY[i] = result;
        }
    }

    // -------------------------------------------------------------------------
    PlotComponent::Information information;

    information.minimumXValue = 0;
    information.maximumXValue = (N - 1.0) / sampleRate * 1000.0;
    information.gridXSize = 4.0;

    information.minimumYValue = -1.5;
    information.maximumYValue = 1.5;
    information.gridYSize = 0.25;

    information.displayGrid = true;
    information.strLabelX = "time (ms)";
    information.strLabelY = "amplitude";
    
    thePlotComponent->setInformation (information);

    // -------------------------------------------------------------------------
    thePlotComponent->pushData (bufferData, N, 1);
    
    triggerAsyncUpdate();
}
