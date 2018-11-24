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

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../SessionComponents.h"
#include "../../Assets/PlotComponent.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
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
