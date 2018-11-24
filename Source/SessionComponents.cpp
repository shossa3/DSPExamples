/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "SessionComponents.h"


//==============================================================================
SettingsComponent::SettingsComponent (AudioDeviceManager *adm) : SessionComponent ("Settings", adm)
{
    addAndMakeVisible (selectorComponent = new AudioDeviceSelectorComponent (*deviceManager, 0, 2, 0, 2, false, false, true, true));
}

SettingsComponent::~SettingsComponent()
{

}

void SettingsComponent::resized()
{
    selectorComponent->setBounds (getLocalBounds());
}


//==============================================================================
AboutComponent::AboutComponent (AudioDeviceManager *adm) : SessionComponent ("About", adm)
{

}

AboutComponent::~AboutComponent()
{

}

void AboutComponent::resized()
{
    
}

void AboutComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (Font (16.f));

    String strTemp = "Audio Developer Conference 2018 Workshop\n";
    strTemp += "Designing digital effects with state-of-the-art DSP and audio filter algorithms\n\n";

    strTemp += "Application and workshop designed by Ivan COHEN\n";
    strTemp += "With the help, advice and trust from the JUCE team and Zsolt GARAMVOLGYI\n";
    strTemp += "Copyright (c) Ivan COHEN 2017-2018\n\n";

    g.drawFittedText (strTemp, getLocalBounds().reduced (20), Justification::topLeft, 20);
}


//==============================================================================
JavascriptEngineComponent::JavascriptEngineComponent (AudioDeviceManager *adm) : SessionComponent ("Javascript Playground", adm)
{
    addAndMakeVisible (editor = new CodeEditorComponent (codeDocument, nullptr));
    editor->setFont ({ Font::getDefaultMonospacedFontName(), 14.0f, Font::plain });
    editor->setTabSize (4, true);

    addAndMakeVisible (outputDisplay = new TextEditor());
    outputDisplay->setMultiLine (true);
    outputDisplay->setReadOnly (true);
    outputDisplay->setCaretVisible (false);
    outputDisplay->setFont ({ Font::getDefaultMonospacedFontName(), 14.0f, Font::plain });
    
    codeDocument.addListener (this);

    editor->loadContent (
        "/**\n"
        "    Javascript! In this simple demo, the native\n"
        "    code provides an object called 'Demo' which\n"
        "    has a method 'print' that writes to the\n"
        "    console below...\n"
        "*/\n\n"

        "Demo.print (\"Newton-Raphson's algorithm in JUCE + Javascript!\");\n"
        "Demo.print (\"Calculus of square root of five\");\n"
        "Demo.print (\"\");\n\n"

        "function square_root_five_newton_raphson (x)\n"
        "{\n"
        "    var g = x * x - 5;    // expression of g in g (x) = 0 = x*x - 5\n"
        "    var dg = 2 * x;       // derivative of g (x)\n"
        "    x = x - g / dg;       // Newton-Raphson's algorithm application\n"
        "    return x;\n"
        "}\n\n"

        "var x_0 = 1;\n\n"

        "Demo.print (\"Initial value = \" + x_0);\n"
        "Demo.print (\"\");\n\n"

        "var x = x_0;\n\n"

        "for (var i = 1; i < 10; ++i)\n"
        "{\n"
        "    x = square_root_five_newton_raphson (x);\n"
        "    Demo.print (\"Iteration \" + i + \" : x = \" + x);\n"
        "}\n\n"

        "Demo.print (\"\");\n"
        "Demo.print (\"Exact value : x = \" + Math.sqrt (5));\n"
    );
}

JavascriptEngineComponent::~JavascriptEngineComponent()
{

}

void JavascriptEngineComponent::resized()
{
    auto r = getLocalBounds().reduced (8);

    editor->setBounds (r.removeFromTop (proportionOfHeight (0.6f)));
    outputDisplay->setBounds (r.withTrimmedTop (8));
}

void JavascriptEngineComponent::paint (Graphics &g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void JavascriptEngineComponent::codeDocumentTextInserted (const String&, int)
{ 
    startTimer (300); 
}

void JavascriptEngineComponent::codeDocumentTextDeleted (int, int)
{ 
    startTimer (300); 
}

void JavascriptEngineComponent::consoleOutput (const String& message)
{
    outputDisplay->moveCaretToEnd();
    outputDisplay->insertTextAtCaret (message + newLine);
}

void JavascriptEngineComponent::timerCallback()
{
    stopTimer();
    runScript();
}

void JavascriptEngineComponent::runScript()
{
    outputDisplay->clear();
    
    JavascriptEngine engine;
    engine.maximumExecutionTime = RelativeTime::seconds (5);
    engine.registerNativeObject ("Demo", new DemoClass (*this));

    auto startTime = Time::getMillisecondCounterHiRes();

    auto result = engine.execute (codeDocument.getAllContent());

    auto elapsedMs = Time::getMillisecondCounterHiRes() - startTime;

    if (result.failed())
        outputDisplay->setText (result.getErrorMessage());
    else
        outputDisplay->insertTextAtCaret ("\n (Execution time: " + String (elapsedMs, 2) + " milliseconds)");
}
