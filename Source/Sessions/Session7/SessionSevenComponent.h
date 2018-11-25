/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../SessionComponents.h"
#include "../../Assets/PlotComponent.h"
#include "../Session6/SessionSixDSP.h"
#include "SessionSevenDSP.h"
#include "../../Assets/LowFrequencyOscillator.h"


//==============================================================================

class SessionSevenComponent : public SessionComponent, public Slider::Listener, 
    public ComboBox::Listener
{
public:
    //==============================================================================
    enum ModulationType
    {
        Modulation_None = 0,
        Modulation_LFO_Triangle,
        Modulation_LFO_Square,
        Modulation_Envelope_Up,
        Modulation_Envelope_Down
    };

    enum ModelType
    {
        Model_SVFNL_Naive = 0,
        Model_MoogFilter_JUCE,
        Model_DiodeClipper_NewtonRaphson
    };
    
    //==============================================================================
    SessionSevenComponent (AudioDeviceManager *adm);
    ~SessionSevenComponent();

    //==============================================================================
    void paint (Graphics &g) override;
    void resized() override;

    //==============================================================================
    void sliderValueChanged (Slider *slider) override;
    void comboBoxChanged (ComboBox *combo) override;
    
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseAudioResources() override;
    void reset() override;
        
private:
    //==============================================================================
    void timerCallback() override;
    void run() override;
    void updateVisibility();
    
    //==============================================================================
    void updateProcessing();

    //==============================================================================
    AudioBuffer<float> bufferModulation;
    LowFrequencyOscillator theLFO;
    LinearSmoothedValue<float> depthVolume, gainVolume;
    EnvelopeFollower theEnvelopeFollower;
    TPTFilter1stOrder theSlewFilter;

    //==============================================================================
    ScopedPointer<dsp::Oversampling<float>> theOversampling;
    
    //==============================================================================
    SVFLowpassFilterNL theFiltersNL[2];
    dsp::LadderFilter<float> theMoogFilter;
    DiodeClipper theDiodeClipper[2];
    float centralFreq;

    bool isActive = false;

    //==============================================================================
    std::atomic<float> lastEnvValue;
    
    int modulationType, modelType;
        
    //==============================================================================
    ScopedPointer<PlotComponent> thePlotComponent;
    AudioBuffer<float> bufferData;
    
    //==============================================================================
    ScopedPointer<Slider> sliderGain;    
    ScopedPointer<Slider> sliderFrequency, sliderResonance, sliderModulation;
    ScopedPointer<ComboBox> comboModel, comboModulationType;
    ScopedPointer<Slider> sliderLFOFrequency, sliderEnvAttack, sliderEnvRelease, sliderEnvSens;

    ScopedPointer<Label> labelGain;
    ScopedPointer<Label> labelFrequency, labelResonance, labelModulation; 
    ScopedPointer<Label> labelModel, labelModulationType;
    ScopedPointer<Label> labelLFOFrequency, labelEnvAttack, labelEnvRelease, labelEnvSens;
        
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionSevenComponent)
};
