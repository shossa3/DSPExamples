/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


class PlotComponent : public Component, public AsyncUpdater
{
public:

    struct Information
    {
        String strTitle;
        String strLabelX;
        String strLabelY;

        double minimumXValue = 0;
        double maximumXValue = 0;
        double minimumYValue = 0;
        double maximumYValue = 0;

        bool isXAxisLogarithmic = false;
        
        bool displayGrid = true;
        double gridXSize = 0;
        double gridYSize = 0;
        bool gridXMustStartAtZero = true;
        bool gridYMustStartAtZero = true;

        double numOctavesMaxFullGridLog = 6.0;
        
        bool displayGridXValues = true;
        bool displayGridYValues = true;
        bool displayGridYValuesReverted = false;
    };


    struct ColourIDs
    {
        Colour backgroundColour = Colours::white;
        Colour outlineColour = Colours::black;

        Array<Colour> dataColours = { Colour (0xFF0072BD), Colour (0xFFD95319), Colour (0xFFEDB120) };

        Colour gridColour = Colours::lightgrey;
        Colour markerColour = Colours::black;

        Colour textColour = Colours::black;
    };


    struct PlotLayout
    {
        float marginX = 0.f;
        float marginRight = 60.f;
        float marginY = 0.f;
        float marginBottom = 0.f;
        float markerSize = 4.f;
        
        float titleFontSize = 20.f;
        float titleSize = 60.f;
        float titleNoSize = 10.f;

        float labelFontSize = 16.f;
        float labelXSize = 30.f;
        float labelYSize = 30.f;

        float valuesFontSize = 14.f;
        float valuesXSize = 30.f;
        float valuesYSize = 40.f;
        float valuesLength = 50.f;
        float valuesYMargin = 5.f;

        float sizePoint = 6.f;

        Font fontPlotComponent = Font();
    };
    
    //==============================================================================
    PlotComponent();

    ~PlotComponent();
        
    //==============================================================================
    /** Sets the data in the Information structure. */
    void setInformation (const Information &newInfo);

    /** Returns the data in the Information structure. */
    const Information getInformation() const;

    //==============================================================================
    /** Sets the data in the ColourIDs structure. */
    void setColourIDs (const ColourIDs &newInfo);
    
    /** Returns the data in the ColourIDs structure. */
    const ColourIDs getColourIDs() const;

    //==============================================================================
    /** Sets the data in the PlotLayout structure. */
    void setPlotLayout (const PlotLayout &newInfo);
    
    /** Returns the data in the PlotLayout structure. */
    const PlotLayout getPlotLayout() const;

    /** Pushes the data to display (several outputs). */
    void pushData (const AudioBuffer<float> &dataXY, int numPoints, int numEntries);
    
private:
    //==============================================================================
    /** @internal */
    void paint (Graphics&) override;

    /** @internal */
    void resized() override;

    /** @internal */
    void handleAsyncUpdate() override;

    //==============================================================================
    /** @internal */
    void paintTitle (Graphics&, Rectangle<int>);

    /** @internal */
    void paintValuesX (Graphics&, Rectangle<int>);
    
    /** @internal */
    void paintValuesXLog (Graphics&, Rectangle<int>);
    
    /** @internal */
    void paintValuesY (Graphics&, Rectangle<int>);

    /** @internal */
    void paintLabelsX (Graphics&, Rectangle<int>);
    
    /** @internal */
    void paintLabelsY (Graphics&, Rectangle<int>);

    /** @internal */
    void paintGridX (Graphics&, Rectangle<int>);
    
    /** @internal */
    void paintGridXLog (Graphics&, Rectangle<int>);
    
    /** @internal */
    void paintGridY (Graphics&, Rectangle<int>);

    /** @internal */
    void paintData (Graphics&, Rectangle<int>);

    //==============================================================================
    struct DataStructure
    {
        DataStructure()
        {
            numPoints.store (0);
            numEntries.store (0);
        }
        
        Array<float> dataX;
        Array<Array<float>> dataY;

        std::atomic<int> numPoints;
        std::atomic<int> numEntries;
    };
    
    //==============================================================================
    CriticalSection processLock, dataLock;
    
    Information information;
    ColourIDs colourIDs;
    PlotLayout plotLayout;
    DataStructure dataStructure;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponent)
    
};





