/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionFourComponent.h"
#include "../../Assets/LowFrequencyOscillator.h"


//==============================================================================
SessionFourComponent::SessionFourComponent (AudioDeviceManager *adm) : SessionComponent ("Compression", adm)
{
    // -------------------------------------------------------------------------
    addAndMakeVisible (thePlotComponent = new PlotComponent());
        
    // -------------------------------------------------------------------------
    addAndMakeVisible (sliderThreshold = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderThreshold->setRange (-80.0, 0.0);
    sliderThreshold->setValue (0.0, dontSendNotification);
    sliderThreshold->setDoubleClickReturnValue (true, 0.0);
    sliderThreshold->addListener (this);
    
    addAndMakeVisible (sliderRatio = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderRatio->setRange (1.0, 100.0);
    sliderRatio->setSkewFactor (0.2);
    sliderRatio->setValue (1.0, dontSendNotification);
    sliderRatio->addListener (this);

    addAndMakeVisible (sliderAttack = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderAttack->setRange (0.1, 1000.0);
    sliderAttack->setSkewFactor (0.2);
    sliderAttack->setValue (0.1, dontSendNotification);
    sliderAttack->addListener (this);

    addAndMakeVisible (sliderRelease = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderRelease->setRange (10.0, 10000.0);
    sliderRelease->setSkewFactor (0.2);
    sliderRelease->setValue (50.0, dontSendNotification);
    sliderRelease->addListener (this);

    // -------------------------------------------------------------------------
    addAndMakeVisible (comboEnvelopeEnabled = new ComboBox());
    comboEnvelopeEnabled->addItemList ({ "Disabled", "Enabled" }, 1);
    comboEnvelopeEnabled->addListener (this);
    comboEnvelopeEnabled->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (labelThreshold = new Label ("", "Threshold"));
    labelThreshold->setJustificationType (Justification::centred);
    labelThreshold->attachToComponent (sliderThreshold, false);

    addAndMakeVisible (labelRatio = new Label ("", "Ratio"));
    labelRatio->setJustificationType (Justification::centred);
    labelRatio->attachToComponent (sliderRatio, false);

    addAndMakeVisible (labelAttack = new Label ("", "Attack"));
    labelAttack->setJustificationType (Justification::centred);
    labelAttack->attachToComponent (sliderAttack, false);

    addAndMakeVisible (labelRelease = new Label ("", "Release"));
    labelRelease->setJustificationType (Justification::centred);
    labelRelease->attachToComponent (sliderRelease, false);

    addAndMakeVisible (labelEnvelopeEnabled = new Label ("", "Envelope"));
    labelEnvelopeEnabled->setJustificationType (Justification::centred);
    labelEnvelopeEnabled->attachToComponent (comboEnvelopeEnabled, false);

    // -------------------------------------------------------------------------
    updateVisibility();
    triggerAsyncUpdate();
}

SessionFourComponent::~SessionFourComponent()
{
    
}

void SessionFourComponent::paint (Graphics &g)
{
    auto rect = getLocalBounds();

    g.setColour (Colours::white);
    g.fillRect (rect.removeFromTop (getHeight() - 170));
    
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (rect);
}

void SessionFourComponent::resized()
{
    Rectangle<int> rect = getLocalBounds();
    
    auto rectTop = rect.removeFromTop (getHeight() - 170);
    thePlotComponent->setBounds (rectTop.reduced (10, 10));

    rect.removeFromTop (40);
    auto rectMid = rect.removeFromTop (100).reduced (10, 0);

    comboEnvelopeEnabled->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24));

    sliderThreshold->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200, 0));
    sliderRatio->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 100, 0));
    sliderAttack->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 200, 0));
    sliderRelease->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 300, 0));
}

void SessionFourComponent::updateVisibility()
{
    auto isEnvelopeEnabled = comboEnvelopeEnabled->getSelectedId() == 2;

    sliderAttack->setVisible (isEnvelopeEnabled);
    sliderRelease->setVisible (isEnvelopeEnabled);
}

//==============================================================================
void SessionFourComponent::sliderValueChanged (Slider *slider)
{
    if (slider == sliderThreshold || slider == sliderRatio)
    {
        mustUpdateProcessing = true;
    }

    else if (slider == sliderAttack || slider == sliderRelease)
    {
        mustUpdateProcessing = true;
    }
}

void SessionFourComponent::comboBoxChanged (ComboBox *combo)
{
    if (combo == comboEnvelopeEnabled)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
}

//==============================================================================
void SessionFourComponent::prepareToPlay (int samplesPerBlockExpected, double sR)
{
    SessionComponent::prepareToPlay (samplesPerBlockExpected, sR);

    theCompressor.ballisticsFilter.initProcessing (sR);
    
    updateProcessing();
    reset();

    isActive = true;
}

void SessionFourComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (mustUpdateProcessing)
        updateProcessing();

    ScopedNoDenormals noDenormals;

    auto startSample = bufferToFill.startSample;
    auto numSamples = bufferToFill.numSamples;
    auto numChannels = jmax (2, bufferToFill.buffer->getNumChannels());

    jassert (numSamples > 0);
    jassert (numChannels > 0);
        
    for (auto i = 0; i < numSamples; i++)
    {
        auto sideInput = 0.f;

        // Detector (peak)
        for (auto channel = 0; channel < numChannels; channel++)
            sideInput = jmax (sideInput, std::abs (bufferToFill.buffer->getSample (channel, i + startSample)));
                                
        // Ballistics filter for envelope generation
        auto env = theCompressor.mustUseBallistics ? theCompressor.ballisticsFilter.processSingleSample (sideInput) : sideInput; 

        // Compressor transfer function
        auto cv = (env <= theCompressor.thrlin ? 1.f : std::pow (env / theCompressor.thrlin, theCompressor.ratioinv - 1.f));

        // Auto make-up gain
        cv *= theCompressor.makeupgain;
            
        // Processing
        for (auto channel = 0; channel < numChannels; channel++)
            bufferToFill.buffer->applyGain (channel, i + startSample, 1, cv);
    }
}

void SessionFourComponent::releaseAudioResources()
{
    reset();
}

void SessionFourComponent::reset()
{
    theCompressor.ballisticsFilter.reset();
}

void SessionFourComponent::updateProcessing()
{
    mustUpdateProcessing = false;

    theCompressor.thrlin = (float) Decibels::decibelsToGain (sliderThreshold->getValue());
    theCompressor.ratioinv = 1.f / (float)sliderRatio->getValue();
    theCompressor.makeupgain = Decibels::decibelsToGain (-((float)sliderThreshold->getValue() * (1.f - theCompressor.ratioinv)) / 2.f);

    theCompressor.ballisticsFilter.setAttackTime ((float)sliderAttack->getValue());
    theCompressor.ballisticsFilter.setReleaseTime ((float)sliderRelease->getValue());
    theCompressor.mustUseBallistics = comboEnvelopeEnabled->getSelectedId() == 2;
}

//==============================================================================
void SessionFourComponent::timerCallback()
{
    stopThread (1000);
    startThread();
}

void SessionFourComponent::run()
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
    LowFrequencyOscillator theLFO;

    theLFO.setFrequency (50.0);
    theLFO.setType (LowFrequencyOscillator::lfoSquare);
    theLFO.initProcessing (sampleRate);
    theLFO.reset (7.f * float_Pi / 4.f);

    samples = bufferData.getWritePointer (1);

    for (auto i = 0; i < N; i++)
        samples[i] = theLFO.getNextSample();

    if (threadShouldExit())
        return;

    // -------------------------------------------------------------------------
    BallisticsFilter theBallisticsFilter;

    theBallisticsFilter.setAttackTime ((float)sliderAttack->getValue());
    theBallisticsFilter.setReleaseTime ((float)sliderRelease->getValue());
    theBallisticsFilter.initProcessing (sampleRate);

    auto *dataY = bufferData.getWritePointer (2);
    auto isEnvelopeEnabled = comboEnvelopeEnabled->getSelectedId() == 2;
    
    for (auto i = 0; i < N; i++)
    {
        if (isEnvelopeEnabled)
            dataY[i] = theBallisticsFilter.processSingleSample (samples[i]);
        else
            dataY[i] = samples[i];
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
    thePlotComponent->pushData (bufferData, N, 2);
    
    triggerAsyncUpdate();
}
