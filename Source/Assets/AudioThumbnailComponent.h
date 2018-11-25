/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class AudioThumbnailComponent    : public Component,
                                   public ChangeBroadcaster,
                                   private ChangeListener,
                                   private Timer
{
public:
    AudioThumbnailComponent (AudioDeviceManager& adm, AudioFormatManager& afm)
        : audioDeviceManager (adm),
          thumbnailCache (5),
          thumbnail (128, afm, thumbnailCache)
    {
        thumbnail.addChangeListener (this);
    }

    ~AudioThumbnailComponent()
    {
        thumbnail.removeChangeListener (this);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (Colour (0xff495358));

        g.setColour (Colours::white);

        if (thumbnail.getTotalLength() > 0.0)
        {
            thumbnail.drawChannels (g, getLocalBounds().reduced (2),
                                    0.0, thumbnail.getTotalLength(), 1.0f);

            g.setColour (Colours::black);
            g.fillRect (static_cast<float> (currentPosition * getWidth()), 0.0f,
                        1.0f, static_cast<float> (getHeight()));
        }
        else
        {
            g.drawFittedText ("No audio file loaded.\nDrop a file here or click the \"Load File...\" button.", getLocalBounds(),
                              Justification::centred, 2);
        }
    }
        
    void setCurrentURL (const URL& u)
    {
        if (currentURL == u)
            return;

        loadURL (u);
    }

    URL getCurrentURL()    { return currentURL; }

    void setTransportSource (AudioTransportSource* newSource)
    {
        transportSource = newSource;

        struct ResetCallback  : public CallbackMessage
        {
            ResetCallback (AudioThumbnailComponent& o) : owner (o) {}
            void messageCallback() override    { owner.reset(); }

            AudioThumbnailComponent& owner;
        };

        (new ResetCallback (*this))->post();
    }

private:
    AudioDeviceManager& audioDeviceManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    AudioTransportSource* transportSource = nullptr;

    URL currentURL;
    double currentPosition = 0.0;

    //==============================================================================
    void changeListenerCallback (ChangeBroadcaster*) override    { repaint(); }

    void reset()
    {
        currentPosition = 0.0;
        repaint();

        if (transportSource == nullptr)
            stopTimer();
        else
            startTimerHz (25);
    }

    void loadURL (const URL& u, bool notify = false)
    {
        if (currentURL == u)
            return;

        currentURL = u;

        InputSource* inputSource = nullptr;

       #if ! JUCE_IOS
        if (u.isLocalFile())
        {
            inputSource = new FileInputSource (u.getLocalFile());
        }
        else
       #endif
        {
            if (inputSource == nullptr)
                inputSource = new URLInputSource (u);
        }

        thumbnail.setSource (inputSource);

        if (notify)
            sendChangeMessage();
    }

    void timerCallback() override
    {
        if (transportSource != nullptr)
        {
            currentPosition = transportSource->getCurrentPosition() / thumbnail.getTotalLength();
            repaint();
        }
    }

    void mouseDrag (const MouseEvent& e) override
    {
        if (transportSource != nullptr)
        {
            const ScopedLock sl (audioDeviceManager.getAudioCallbackLock());

            transportSource->setPosition ((jmax (static_cast<double> (e.x), 0.0) / getWidth())
                                            * thumbnail.getTotalLength());
        }
    }
};
