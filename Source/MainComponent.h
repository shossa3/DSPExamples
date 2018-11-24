/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FileSystem.h"
#include "SessionComponents.h"
#include "./Assets/AudioThumbnailComponent.h"

//==============================================================================
/**
    LookandFeel class of our application
*/
class MainComponentLookAndFeel : public LookAndFeel_V4
{
public:
    //==============================================================================
    MainComponentLookAndFeel()
    {
        setColour (TextButton::ColourIds::buttonColourId, findColour (TextButton::ColourIds::buttonColourId).brighter (0.2f));
        setColour (TextButton::ColourIds::buttonOnColourId, findColour (TextButton::ColourIds::buttonOnColourId).brighter (0.2f));
        setColour (ComboBox::ColourIds::backgroundColourId, findColour (ComboBox::ColourIds::backgroundColourId).brighter());
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponentLookAndFeel)
};


//==============================================================================
/**
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public Component, public ListBoxModel, public AudioSource,
    public Button::Listener, public ComboBox::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged (int lastRowSelected) override;
    void buttonClicked (Button *button) override;
    void comboBoxChanged (ComboBox *combo) override;

private:
    //==============================================================================
    void loadAudioFile (File fileAudio);
    void setAudioChannels (int numInputChannels, int numOutputChannels, const XmlElement* const xml = nullptr);
    void shutdownAudio();

    //==============================================================================
    AudioDeviceManager deviceManager;
    AudioSourcePlayer audioSourcePlayer;

    AudioFormatManager formatManager;
    AudioTransportSource transportSource;
    TimeSliceThread thread;
    ScopedPointer<AudioFormatReaderSource> currentAudioFileSource;
    
    //==============================================================================
    ScopedPointer<ListBox> listDemos;
    OwnedArray<SessionComponent> theSessionComponents;

    //==============================================================================
    ScopedPointer<TextButton> buttonPlay, buttonStop;
    ScopedPointer<ComboBox> comboAudioFiles;
    Array<File> fileListAudio;
    ScopedPointer<AudioThumbnailComponent> theThumbComponent;

    //==============================================================================
    MainComponentLookAndFeel theLF;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
