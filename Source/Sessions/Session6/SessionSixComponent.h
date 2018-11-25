/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../SessionComponents.h"
#include "../../Assets/PlotComponent.h"
#include "SessionSixDSP.h"
#include "../../Assets/LowFrequencyOscillator.h"



class SessionSixComponent : public SessionComponent, public Slider::Listener, 
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

    enum StructureType
    {
        Structure_TDF2 = 0,
        Structure_TDF2_Smoothing,
        Structure_TPT,
        Structure_TPT_Smoothing
    };
    
    //==============================================================================
    SessionSixComponent (AudioDeviceManager *adm);
    ~SessionSixComponent();

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
    LinearSmoothedValue<float> depthVolume;
    EnvelopeFollower theEnvelopeFollower;
    TPTFilter1stOrder theSlewFilter;

    //==============================================================================
    struct FilterStateTPT
    {
        float s1, s2;       // state variable for TPT
    };
    
    struct FilterStateTDF2
    {
        float v1, v2;       // state variable for TDF2        
    };

    FilterStateTDF2 theFiltersTDF2[2];
    FilterStateTPT theFiltersTPT[2];
    bool isActive = false;

    std::atomic<float> lastEnvValue;
    
    int modulationType, algorithmType;
    bool isSmoothing;
    float centralFreq, R2;
        
    //==============================================================================
    ScopedPointer<PlotComponent> thePlotComponent;
    AudioBuffer<float> bufferData;
    
    //==============================================================================
    ScopedPointer<Slider> sliderFrequency, sliderResonance, sliderModulation;
    ScopedPointer<ComboBox> comboAlgorithm, comboModulationType;
    ScopedPointer<Slider> sliderLFOFrequency, sliderEnvAttack, sliderEnvRelease, sliderEnvSens;

    ScopedPointer<Label> labelFrequency, labelResonance, labelModulation; 
    ScopedPointer<Label> labelAlgorithm, labelModulationType;
    ScopedPointer<Label> labelLFOFrequency, labelEnvAttack, labelEnvRelease, labelEnvSens;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionSixComponent)
};
