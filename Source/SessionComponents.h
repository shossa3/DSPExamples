/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class SessionComponent : public Component, public AsyncUpdater,
    public Timer, public Thread
{
public:
    //==============================================================================
    SessionComponent (String strName = String(), AudioDeviceManager *adm = nullptr) : Thread ("")
    {
        setName (strName);
        deviceManager = adm;

        setLookAndFeel (&theLF);
    }

    virtual ~SessionComponent() 
    { 
        setLookAndFeel (nullptr); 

        stopTimer();
        stopThread (1000);
    }

    //==============================================================================
    virtual void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    }

    virtual void resized() override {}

    //==============================================================================
    virtual void prepareToPlay (int samplesPerBlockExpected, double _sampleRate) 
    {
        jassert (_sampleRate > 0);
        
        ignoreUnused (samplesPerBlockExpected);
        sampleRate = _sampleRate;
    }

    virtual void getNextAudioBlock (const AudioSourceChannelInfo& /*bufferToFill*/) {}
    virtual void releaseAudioResources() {}
    virtual void reset() {}

    void visibilityChanged() override
    {
        if (isVisible())
            startTimerHz (30);

        else
        {
            stopTimer();
            stopThread (1000);
        }
    }

protected:
    //==============================================================================
    virtual void timerCallback() override {}
    virtual void run() override {}

    //==============================================================================
    bool mustUpdateProcessing = false;
    double sampleRate = 44100.0;

    //==============================================================================
    AudioDeviceManager *deviceManager;
    LookAndFeel_V4 theLF;

private:
    //==============================================================================
    void handleAsyncUpdate() override
    {
        repaint();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SessionComponent)
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class SettingsComponent : public SessionComponent
{
public:
    //==============================================================================
    SettingsComponent (AudioDeviceManager *adm);
    ~SettingsComponent();

    //==============================================================================
    void resized() override;

private:
    //==============================================================================
    ScopedPointer<AudioDeviceSelectorComponent> selectorComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class AboutComponent : public SessionComponent
{
public:
    //==============================================================================
    AboutComponent (AudioDeviceManager *adm);
    ~AboutComponent();

    //==============================================================================
    void resized() override;
    void paint (Graphics &g) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutComponent)
};


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class JavascriptEngineComponent : public SessionComponent, private CodeDocument::Listener
{
public:
    //==============================================================================
    JavascriptEngineComponent (AudioDeviceManager *adm);
    ~JavascriptEngineComponent();

    //==============================================================================
    void resized() override;
    void paint (Graphics &g) override;

private:
    //==============================================================================
    // This class is used by the script, and provides methods that the JS can call.
    struct DemoClass : public DynamicObject
    {
        DemoClass (JavascriptEngineComponent& demo) : owner (demo)
        {
            setMethod ("print", print);
        }

        static Identifier getClassName() { return "Demo"; }

        static var print (const var::NativeFunctionArgs& args)
        {
            if (args.numArguments > 0)
                if (auto* thisObject = dynamic_cast<DemoClass*> (args.thisObject.getObject()))
                    thisObject->owner.consoleOutput (args.arguments[0].toString());

            return var::undefined();
        }

        JavascriptEngineComponent& owner;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoClass)
    };

    //==============================================================================
    void timerCallback() override;
    void runScript();
    void codeDocumentTextInserted (const String&, int) override;
    void codeDocumentTextDeleted (int, int) override;

    //==============================================================================
    void consoleOutput (const String& message);    
    
    //==============================================================================
    CodeDocument codeDocument;
    ScopedPointer<CodeEditorComponent> editor;
    ScopedPointer<TextEditor> outputDisplay;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JavascriptEngineComponent)
};
