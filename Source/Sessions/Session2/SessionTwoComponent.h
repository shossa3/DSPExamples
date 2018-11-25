/*
  ==============================================================================

  Shaikat Hossain DSPExamples
 
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../SessionComponents.h"
#include "../../Assets/PlotComponent.h"



class SessionTwoComponent : public SessionComponent, public Slider::Listener, 
    public ComboBox::Listener
{
public:
    //==============================================================================
    SessionTwoComponent (AudioDeviceManager *adm);
    ~SessionTwoComponent();

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
        float b0, b1, b2;   // Z transform coefficients (numerator)
        float a1, a2;       // Z transform coefficients (denominator)
    };

    struct FilterState
    {
        float v1, v2;       // state variable for TDF2
    };

    FilterCoefficients theCoefficients;
    FilterState theFilters[2];
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
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionTwoComponent)
};
