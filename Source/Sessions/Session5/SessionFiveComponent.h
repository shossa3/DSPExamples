/*
  ==============================================================================

   Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../SessionComponents.h"
#include "../../Assets/PlotComponent.h"
#include "../Session4/SessionFourDSP.h"
#include "SessionFiveDSP.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class SessionFiveComponent : public SessionComponent, public Slider::Listener, 
    public ComboBox::Listener
{
public:
    //==============================================================================
    enum Smoothing
    {
        Smoothing_NoSmoothing = 0,
        Smoothing_Ballistics,
        Smoothing_BallisticsSmoothedValue,
        Smoothing_BallisticsSmoothedValueFilter,
        Smoothing_BallisticsSmoothedValueLinearSlewLimiting
    };
    
    //==============================================================================
    SessionFiveComponent (AudioDeviceManager *adm);
    ~SessionFiveComponent();

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
        
    //==============================================================================
    void updateProcessing();
    void updateVisibility();

    //==============================================================================
    struct DynamicsCompressorState
    {
        float thrlin, ratioinv;                 // internal variables
        LinearSmoothedValue<float> makeupgain;  // internal variable
        BallisticsFilter ballisticsFilter;      // internal object
        TPTFilter1stOrder slewFilter;           // internal object
        LinearSlewLimiter slewLimiter;          // internal object
        int smoothingAlgorithm;                 // internal variable
    };

    DynamicsCompressorState theCompressor;
    bool isActive = false;
        
    //==============================================================================
    ScopedPointer<PlotComponent> thePlotComponent;
    AudioBuffer<float> bufferData;
    
    //==============================================================================
    ScopedPointer<Slider> sliderThreshold, sliderRatio, sliderAttack, sliderRelease;
    ScopedPointer<Label> labelThreshold, labelRatio, labelAttack, labelRelease;
    ScopedPointer<ComboBox> comboSmoothingAlgorithm;
    ScopedPointer<Label> labelSmoothingAlgorithm;

    ScopedPointer<Slider> sliderSlewFreq, sliderSlewLinear;
    ScopedPointer<Label> labelSlewFreq, labelSlewLinear;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionFiveComponent)
};
