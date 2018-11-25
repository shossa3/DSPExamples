/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../SessionComponents.h"
#include "../../Assets/PlotComponent.h"


class SessionThreeComponent : public SessionComponent, public Slider::Listener, 
    public ComboBox::Listener
{
public:
    //==============================================================================
    SessionThreeComponent (AudioDeviceManager *adm);
    ~SessionThreeComponent();

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
    struct FilterCoefficients
    {
        float b0, b1, b2, b3, b4;   // Z transform coefficients (numerator)
        float a1, a2, a3, a4;       // Z transform coefficients (denominator)
        int order;
    };

    struct FilterState
    {
        float v1, v2, v3, v4;       // state variable for TDF2
    };

    FilterCoefficients theCoefficients;
    FilterState theFilters[2];
    ScopedPointer<dsp::Oversampling<float>> theOversampling;
    bool isActive = false;
        
    //==============================================================================
    ScopedPointer<PlotComponent> thePlotComponent;
    AudioBuffer<float> bufferData;
    
    //==============================================================================
    ScopedPointer<Slider> sliderFrequency, sliderResonance, sliderGain;
    ScopedPointer<ComboBox> comboType, comboDisplay, comboAlgorithm;
    ScopedPointer<Label> labelFrequency, labelResonance, labelGain; 
    ScopedPointer<Label> labelType, labelDisplay, labelAlgorithm;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionThreeComponent)
};
