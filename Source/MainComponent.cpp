/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "MainComponent.h"

// You need to include all the session components headers you need there
#include "./Sessions/Session1/SessionOneComponent.h"
#include "./Sessions/Session3/SessionThreeComponent.h"
#include "./Sessions/Session4/SessionFourComponent.h"
#include "./Sessions/Session5/SessionFiveComponent.h"
#include "./Sessions/Session6/SessionSixComponent.h"
#include "./Sessions/Session7/SessionSevenComponent.h"


//==============================================================================
MainComponent::MainComponent() : thread ("audio file buffering")
{
    // -------------------------------------------------------------------------
    // Here is the place where we add the session components
    theSessionComponents.add (new SessionOneComponent (&deviceManager));
    theSessionComponents.add (new SessionThreeComponent (&deviceManager));
    theSessionComponents.add (new SessionFourComponent (&deviceManager));
    theSessionComponents.add (new SessionFiveComponent (&deviceManager));
    theSessionComponents.add (new SessionSixComponent (&deviceManager));
    theSessionComponents.add (new SessionSevenComponent (&deviceManager));
    
    theSessionComponents.add (new SettingsComponent (&deviceManager));

    for (auto i = 0; i < theSessionComponents.size(); i++)
        addChildComponent (theSessionComponents[i]);

    // --------------------------------------------------------------------------------
    setLookAndFeel (&theLF);
    
    // --------------------------------------------------------------------------------
    addAndMakeVisible (buttonPlay = new TextButton ("Play"));
    buttonPlay->addListener (this);
    buttonPlay->setConnectedEdges (Button::ConnectedOnRight);

    addAndMakeVisible (buttonStop = new TextButton ("Stop"));
    buttonStop->addListener (this);
    buttonStop->setConnectedEdges (Button::ConnectedOnLeft);

    // --------------------------------------------------------------------------------
    addAndMakeVisible (comboAudioFiles = new ComboBox());
    comboAudioFiles->addListener (this);

    fileListAudio = FileSystem::getAudioList();
    for (auto i = 0; i < fileListAudio.size(); i++)
        comboAudioFiles->addItem (fileListAudio[i].getFileNameWithoutExtension(), i + 1);

    comboAudioFiles->setSelectedId (1, dontSendNotification);

    // -------------------------------------------------------------------------
    addAndMakeVisible (listDemos = new ListBox());
    listDemos->setModel (this);
    listDemos->setColour (ListBox::backgroundColourId, Colour::greyLevel (0.2f));
    
    // -------------------------------------------------------------------------
    formatManager.registerBasicFormats();
    thread.startThread (3);
    currentAudioFileSource = nullptr;

    // -------------------------------------------------------------------------
    addAndMakeVisible (theThumbComponent = new AudioThumbnailComponent (deviceManager, formatManager));
    theThumbComponent->setTransportSource (&transportSource);
    
    // -------------------------------------------------------------------------
    // Here is the place where we set the session component we want to see at startup
    listDemos->selectRow (0);
    setAudioChannels (0, 2);
    setSize (920, 600);
}

MainComponent::~MainComponent()
{
    releaseResources();
    shutdownAudio();
    setLookAndFeel (nullptr);
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);

    for (auto i = 0; i < theSessionComponents.size(); i++)
        theSessionComponents[i]->prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // -----------------------------------------------------------------------------
    bufferToFill.clearActiveBufferRegion();
    transportSource.getNextAudioBlock (bufferToFill);

    // -----------------------------------------------------------------------------
    const int index = listDemos->getSelectedRow();
    theSessionComponents[index]->getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    transportSource.stop();
    transportSource.releaseResources();

    for (auto i = 0; i < theSessionComponents.size(); i++)
        theSessionComponents[i]->releaseAudioResources();
}

void MainComponent::loadAudioFile (File fileAudio)
{
    // unload the previous file source and delete it..
    transportSource.stop();
    transportSource.setSource (nullptr);
    currentAudioFileSource = nullptr;

    AudioFormatReader* reader = formatManager.createReaderFor (fileAudio);

    if (reader != nullptr)
    {
        currentAudioFileSource = new AudioFormatReaderSource (reader, true);

        currentAudioFileSource->setLooping (true);

        // ..and plug it into our transport source
        transportSource.setSource (currentAudioFileSource,
            32768,                   // tells it to buffer this many samples ahead
            &thread,                 // this is the background thread to use for reading-ahead
            reader->sampleRate,		 // allows for sample rate correction
            2);						 // for stereo

        URL theURL = URL (File (fileAudio));
        theThumbComponent->setCurrentURL (theURL);
    }
}

void MainComponent::setAudioChannels (int numInputChannels, int numOutputChannels, const XmlElement* const xml)
{
    String audioError = deviceManager.initialise (numInputChannels, numOutputChannels, xml, true);
    jassert (audioError.isEmpty());

    deviceManager.addAudioCallback (&audioSourcePlayer);
    audioSourcePlayer.setSource (this);

    if (fileListAudio.size() > 0)
        loadAudioFile (fileListAudio[0]);
}

void MainComponent::shutdownAudio()
{
    transportSource.stop();
    thread.stopThread (1000);

    audioSourcePlayer.setSource (nullptr);
    transportSource.setSource (nullptr);
    deviceManager.removeAudioCallback (&audioSourcePlayer);
    deviceManager.closeAudioDevice();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).brighter());
}

void MainComponent::resized()
{
    Rectangle<int> rect = getLocalBounds();

    if (rect.getWidth() > 600)
    {
        listDemos->setBounds (rect.removeFromLeft (210));
        listDemos->setRowHeight (20);
    }
    else
    {
        listDemos->setBounds (rect.removeFromLeft (130));
        listDemos->setRowHeight (30);
    }

    auto fullRect = rect;
    auto rectTop = rect.removeFromTop (100);
    rectTop.reduce (10, 10);

    comboAudioFiles->setBounds (rectTop.removeFromTop (40).withWidth (200).withSizeKeepingCentre (200,24));
    buttonPlay->setBounds (rectTop.withWidth (100).withSizeKeepingCentre (100, 24));
    buttonStop->setBounds (buttonPlay->getBounds().translated (100, 0));

    for (auto i = 0; i < theSessionComponents.size(); i++)
        theSessionComponents[i]->setBounds (i >= theSessionComponents.size() - 2 ? fullRect : rect);

    auto rectThumb = fullRect.reduced (10, 10);
    rectThumb.removeFromLeft (210);
    rectThumb = rectThumb.removeFromTop (80);
    theThumbComponent->setBounds (rectThumb);
}

//==============================================================================
int MainComponent::getNumRows()
{
    return theSessionComponents.size();
}

void MainComponent::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (Colours::deepskyblue);

    g.setFont (Font (13));
    g.setColour (Colours::white);
    g.drawText (theSessionComponents[rowNumber]->getName(), 0, 0, width, height, Justification::centredLeft);
}

void MainComponent::selectedRowsChanged (int lastRowSelected)
{
    for (auto i = 0; i < theSessionComponents.size(); i++)
        theSessionComponents[i]->setVisible (i == lastRowSelected);

    buttonPlay->setVisible (lastRowSelected < theSessionComponents.size() - 2);
    buttonStop->setVisible (lastRowSelected < theSessionComponents.size() - 2);
    comboAudioFiles->setVisible (lastRowSelected < theSessionComponents.size() - 2);
    theThumbComponent->setVisible (lastRowSelected < theSessionComponents.size() - 2);

    if (lastRowSelected >= theSessionComponents.size() - 2)
    {
        transportSource.stop();
    }
}

void MainComponent::buttonClicked (Button *button)
{
    if (button == buttonPlay)
    {
        transportSource.setLooping (true);
        transportSource.start();
    }
    else if (button == buttonStop)
    {
        transportSource.stop();
        transportSource.setPosition (0.0);
    }
}

void MainComponent::comboBoxChanged (ComboBox *combo)
{
    if (combo == comboAudioFiles)
    {
        const int index = comboAudioFiles->getSelectedId() - 1;
        File fileAudio = fileListAudio[index];

        loadAudioFile (fileAudio);
    }
}
