/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionFiveComponent.h"
#include "../../Assets/LowFrequencyOscillator.h"


//==============================================================================
SessionFiveComponent::SessionFiveComponent (AudioDeviceManager *adm) : SessionComponent ("Smoothing", adm)
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
    addAndMakeVisible (sliderSlewFreq = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderSlewFreq->setRange (10.0, 1000.0);
    sliderSlewFreq->setSkewFactor (0.2);
    sliderSlewFreq->setValue (40.0, dontSendNotification);
    sliderSlewFreq->addListener (this);

    addAndMakeVisible (sliderSlewLinear = new Slider (Slider::RotaryVerticalDrag, Slider::TextBoxBelow));
    sliderSlewLinear->setRange (1.0, 10000.0);
    sliderSlewLinear->setSkewFactor (0.2);
    sliderSlewLinear->setValue (400.0, dontSendNotification);
    sliderSlewLinear->addListener (this);

    // -------------------------------------------------------------------------
    addAndMakeVisible (comboSmoothingAlgorithm = new ComboBox());
    comboSmoothingAlgorithm->addItemList ({ "No Envelope", "Simple envelope",
        "Envelope + make-up smoothing", "Smoothing + filtering", 
        "Smoothing + slew limiting"}, 1);
    comboSmoothingAlgorithm->addListener (this);
    comboSmoothingAlgorithm->setSelectedId (1, dontSendNotification);

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

    addAndMakeVisible (labelSlewFreq = new Label ("", "Slew/Freq"));
    labelSlewFreq->setJustificationType (Justification::centred);
    labelSlewFreq->attachToComponent (sliderSlewFreq, false);

    addAndMakeVisible (labelSlewLinear = new Label ("", "Slew/Lin"));
    labelSlewLinear->setJustificationType (Justification::centred);
    labelSlewLinear->attachToComponent (sliderSlewLinear, false);

    addAndMakeVisible (labelSmoothingAlgorithm = new Label ("", "Envelope"));
    labelSmoothingAlgorithm->setJustificationType (Justification::centred);
    labelSmoothingAlgorithm->attachToComponent (comboSmoothingAlgorithm, false);

    // -------------------------------------------------------------------------
    updateVisibility();
    triggerAsyncUpdate();
}

SessionFiveComponent::~SessionFiveComponent()
{
    
}

void SessionFiveComponent::paint (Graphics &g)
{
    auto rect = getLocalBounds();

    g.setColour (Colours::white);
    g.fillRect (rect.removeFromTop (getHeight() - 170));
    
    g.setColour (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillRect (rect);
}

void SessionFiveComponent::resized()
{
    Rectangle<int> rect = getLocalBounds();
    
    auto rectTop = rect.removeFromTop (getHeight() - 170);
    thePlotComponent->setBounds (rectTop.reduced (10, 10));

    rect.removeFromTop (40);
    auto rectMid = rect.removeFromTop (100).reduced (10, 0);

    comboSmoothingAlgorithm->setBounds (rectMid.withWidth (160).withSizeKeepingCentre (160, 24));

    sliderThreshold->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200, 0));
    sliderRatio->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 100, 0));
    sliderAttack->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 200, 0));
    sliderRelease->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 300, 0));
    sliderSlewFreq->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 400, 0));
    sliderSlewLinear->setBounds (rectMid.withWidth (80).withSizeKeepingCentre (80, 80).translated (200 + 400, 0));
}

void SessionFiveComponent::updateVisibility()
{
    auto index = comboSmoothingAlgorithm->getSelectedId() - 1;

    sliderAttack->setVisible (index >= Smoothing_Ballistics);
    sliderRelease->setVisible (index >= Smoothing_Ballistics);
    sliderSlewFreq->setVisible (index == Smoothing_BallisticsSmoothedValueFilter);
    sliderSlewLinear->setVisible (index == Smoothing_BallisticsSmoothedValueLinearSlewLimiting);
}

//==============================================================================
void SessionFiveComponent::sliderValueChanged (Slider *slider)
{
    if (slider == sliderThreshold || slider == sliderRatio)
    {
        mustUpdateProcessing = true;
    }

    else if (slider == sliderAttack || slider == sliderRelease)
    {
        mustUpdateProcessing = true;
    }
    else if (slider == sliderSlewFreq || slider == sliderSlewLinear)
    {
        mustUpdateProcessing = true;
    }
}

void SessionFiveComponent::comboBoxChanged (ComboBox *combo)
{
    if (combo == comboSmoothingAlgorithm)
    {
        mustUpdateProcessing = true;

        updateVisibility();
    }
}

//==============================================================================
void SessionFiveComponent::prepareToPlay (int samplesPerBlockExpected, double sR)
{
    SessionComponent::prepareToPlay (samplesPerBlockExpected, sR);

    theCompressor.ballisticsFilter.initProcessing (sR);
    theCompressor.slewFilter.initProcessing (sR);
        
    updateProcessing();
    reset();

    isActive = true;
}

void SessionFiveComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
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
        auto env = sideInput;
        
        if (theCompressor.smoothingAlgorithm >= Smoothing_Ballistics)
            env = theCompressor.ballisticsFilter.processSingleSample (sideInput);

        // Compressor transfer function
        auto cv = (env <= theCompressor.thrlin ? 1.f : std::pow (env / theCompressor.thrlin, theCompressor.ratioinv - 1.f));

        // Slew limiting
        if (theCompressor.smoothingAlgorithm == Smoothing_BallisticsSmoothedValueFilter)
            cv = theCompressor.slewFilter.processSingleSampleRaw (cv);
        else if (theCompressor.smoothingAlgorithm == Smoothing_BallisticsSmoothedValueLinearSlewLimiting)
            cv = theCompressor.slewLimiter.getSample (cv);
        
        // Auto make-up gain (smoothed)
        if (theCompressor.smoothingAlgorithm >= Smoothing_BallisticsSmoothedValue)
            cv *= theCompressor.makeupgain.getNextValue();
        else
            cv *= theCompressor.makeupgain.getTargetValue();
            
        // Processing
        for (auto channel = 0; channel < numChannels; channel++)
            bufferToFill.buffer->applyGain (channel, i + startSample, 1, cv);
    }
}

void SessionFiveComponent::releaseAudioResources()
{
    reset();
}

void SessionFiveComponent::reset()
{
    theCompressor.ballisticsFilter.reset();
    theCompressor.slewFilter.reset();
    theCompressor.slewLimiter.reset();
    theCompressor.makeupgain.reset (sampleRate, 0.05);
}

void SessionFiveComponent::updateProcessing()
{
    mustUpdateProcessing = false;

    theCompressor.thrlin = (float) Decibels::decibelsToGain (sliderThreshold->getValue());
    theCompressor.ratioinv = 1.f / (float)sliderRatio->getValue();
    theCompressor.makeupgain.setValue (Decibels::decibelsToGain (-((float)sliderThreshold->getValue() * (1.f - theCompressor.ratioinv)) / 2.f));

    theCompressor.ballisticsFilter.setAttackTime ((float)sliderAttack->getValue());
    theCompressor.ballisticsFilter.setReleaseTime ((float)sliderRelease->getValue());
    theCompressor.smoothingAlgorithm = comboSmoothingAlgorithm->getSelectedId() - 1;

    theCompressor.slewFilter.setCutoffFrequency ((float)sliderSlewFreq->getValue());
    theCompressor.slewFilter.setType (TPTFilter1stOrder::TypeLowPass);
    theCompressor.slewLimiter.setSlew (sampleRate, (float)sliderSlewLinear->getValue(), (float)sliderSlewLinear->getValue());
}

//==============================================================================
void SessionFiveComponent::timerCallback()
{
    stopThread (1000);
    startThread();
}

void SessionFiveComponent::run()
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
    theBallisticsFilter.reset();

    TPTFilter1stOrder theSlewFilter;
    theSlewFilter.setCutoffFrequency ((float) sliderSlewFreq->getValue());
    theSlewFilter.setType (TPTFilter1stOrder::TypeLowPass);
    theSlewFilter.initProcessing (sampleRate);
    theSlewFilter.reset();

    LinearSlewLimiter theSlewLimiter;
    theSlewLimiter.setSlew (sampleRate, (float)sliderSlewLinear->getValue(), (float)sliderSlewLinear->getValue());
    theSlewLimiter.reset();

    auto algorithm = comboSmoothingAlgorithm->getSelectedId() - 1;

    auto *dataY = bufferData.getWritePointer (2);
        
    for (auto i = 0; i < N; i++)
    {
        auto result = samples[i];
        
        if (algorithm >= Smoothing_Ballistics)
            result = theBallisticsFilter.processSingleSample (result);

        if (algorithm == Smoothing_BallisticsSmoothedValueFilter)
            result = theSlewFilter.processSingleSampleRaw (result);

        if (algorithm == Smoothing_BallisticsSmoothedValueLinearSlewLimiting)
            result = theSlewLimiter.getSample (result);
        
        dataY[i] = result;
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
