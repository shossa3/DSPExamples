/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#include "PlotComponent.h"
#include "StringFunctions.h"


//==============================================================================
/*
    This structure gives a few functions interesting for visualization of data.
*/
struct PlotComponentHelpers
{
    static const double getPixelXPositionForValue (double valueX, int width, double minimum, double maximum)
    {
        return 0.5 + (width - 1.0) * (valueX - minimum) / (maximum - minimum);
    }

    static const double getPixelXPositionForValueLog (double valueX, int width, double minimum, double maximum)
    {
        return 0.5 + (width - 1.0) * (std::log10 (valueX) - std::log10 (minimum)) / (std::log10 (maximum) - std::log10 (minimum));
    }

    static const double getPixelYPositionForValue (double valueY, int height, double minimum, double maximum)
    {
        return 0.5 + (height - 1.0) * (1.0 - (valueY - minimum) / (maximum - minimum));
    }

    static const double getIntegerPixelXPositionForValue (double valueX, int width, double minimum, double maximum)
    {
        return roundToInt (getPixelXPositionForValue (valueX, width, minimum, maximum) - 0.5) + 0.5;
    }

    static const double getIntegerPixelXPositionForValueLog (double valueX, int width, double minimum, double maximum)
    {
        return roundToInt (getPixelXPositionForValueLog (valueX, width, minimum, maximum) - 0.5) + 0.5;
    }

    static const double getIntegerPixelYPositionForValue (double valueY, int height, double minimum, double maximum)
    {
        return roundToInt (getPixelYPositionForValue (valueY, height, minimum, maximum) - 0.5) + 0.5;
    }
};



//==============================================================================
PlotComponent::PlotComponent()
{
}

PlotComponent::~PlotComponent()
{
}

void PlotComponent::resized()
{
}

//==============================================================================
void PlotComponent::setInformation (const Information &newInfo)
{
    jassert (newInfo.gridXSize >= 0);
    jassert (newInfo.gridYSize >= 0);

    jassert (newInfo.minimumXValue < newInfo.maximumXValue);
    jassert (newInfo.minimumYValue < newInfo.maximumYValue);

    jassert (!(newInfo.isXAxisLogarithmic && newInfo.minimumXValue == 0));

    const ScopedLock myScopedLock (processLock);
    information = newInfo;

    triggerAsyncUpdate();
}

const PlotComponent::Information PlotComponent::getInformation() const
{
    const ScopedLock myScopedLock (processLock);
    return information;
}

void PlotComponent::setColourIDs (const PlotComponent::ColourIDs &newInfo)
{
    const ScopedLock myScopedLock (processLock);
    colourIDs = newInfo;

    triggerAsyncUpdate();
}

const PlotComponent::ColourIDs PlotComponent::getColourIDs() const
{
    const ScopedLock myScopedLock (processLock);
    return colourIDs;
}

void PlotComponent::setPlotLayout (const PlotComponent::PlotLayout &newInfo)
{
    jassert (newInfo.marginX >= 0.f);
    jassert (newInfo.marginRight >= 0.f);
    jassert (newInfo.marginY >= 0.f);
    jassert (newInfo.marginBottom >= 0.f);
    jassert (newInfo.markerSize >= 0.f);

    jassert (newInfo.titleFontSize > 0.f);
    jassert (newInfo.titleSize >= 0.f);
    jassert (newInfo.titleNoSize >= 0.f);

    jassert (newInfo.labelFontSize > 0.f);
    jassert (newInfo.labelXSize >= 0.f);
    jassert (newInfo.labelYSize >= 0.f);

    jassert (newInfo.valuesFontSize > 0.f);
    jassert (newInfo.valuesXSize >= 0.f);
    jassert (newInfo.valuesYSize >= 0.f);
    jassert (newInfo.valuesLength >= 0.f);
    jassert (newInfo.valuesYMargin >= 0.f);
    
    const ScopedLock myScopedLock (processLock);
    plotLayout = newInfo;

    triggerAsyncUpdate();
}

const PlotComponent::PlotLayout PlotComponent::getPlotLayout() const
{
    const ScopedLock myScopedLock (processLock);
    return plotLayout;
}

void PlotComponent::pushData (const AudioBuffer<float> &dataXY, int numPoints, int numEntries)
{
    jassert (numPoints > 0);
    jassert (numEntries >= 1);
    jassert (dataXY.getNumSamples() >= numPoints);
    jassert (dataXY.getNumChannels() >= numEntries + 1);

    const ScopedLock myScopedLock (dataLock);

    dataStructure.dataX.clear();
    dataStructure.dataX.addArray (dataXY.getReadPointer (0), numPoints);

    dataStructure.dataY.clear();
    for (auto i = 0; i < numEntries; i++)
        dataStructure.dataY.add (Array<float> (dataXY.getReadPointer (1 + i), numPoints));

    dataStructure.numPoints.store (numPoints);
    dataStructure.numEntries.store (numEntries);
}

//==============================================================================
void PlotComponent::handleAsyncUpdate()
{
    repaint();
}

void PlotComponent::paint (Graphics& g)
{
    // -------------------------------------------------------------------------------------------------------------
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    auto canDisplayData = true;
    auto canDisplayGrid = true;

    // -------------------------------------------------------------------------------------------------------------
    if (currentInformation.isXAxisLogarithmic && currentInformation.minimumXValue <= 0)
    {
        // Logarithmic means minimum should be higher than zero !
        canDisplayData = false;
        canDisplayGrid = false;
    }
    else if (currentInformation.minimumXValue == currentInformation.maximumXValue)
    {
        canDisplayData = false;
        canDisplayGrid = false;
    }
    else if (currentInformation.minimumYValue == currentInformation.maximumYValue)
    {
        canDisplayData = false;
        canDisplayGrid = false;
    }

    if (dataStructure.numPoints == 0)
        canDisplayData = false;

        
    // -------------------------------------------------------------------------------------------------------------
    Rectangle<int> rect = getLocalBounds();

    rect.removeFromLeft (roundToInt (currentLayout.marginX));
    rect.removeFromRight (roundToInt (currentLayout.marginRight));
    rect.removeFromTop (roundToInt (currentLayout.marginY));
    rect.removeFromBottom (roundToInt (currentLayout.marginBottom));

    // Title
    if (information.strTitle.isNotEmpty())
    {
        Rectangle<int> rectTitle = rect.removeFromTop (roundToInt (currentLayout.titleSize));

        if (information.strLabelY.isNotEmpty())
            rectTitle.removeFromLeft (roundToInt (currentLayout.labelYSize));

        if (information.displayGridYValues)
            rectTitle.removeFromLeft (roundToInt (currentLayout.valuesYSize));

        paintTitle (g, rectTitle);
    }
    else
        rect.removeFromTop (roundToInt (currentLayout.titleNoSize));

    // Label X
    if (information.strLabelX.isNotEmpty())
    {
        Rectangle<int> rectLabel = rect.removeFromBottom (roundToInt (currentLayout.labelXSize));

        if (information.strLabelY.isNotEmpty())
            rectLabel.removeFromLeft (roundToInt (currentLayout.labelYSize));

        if (information.displayGridYValues)
            rectLabel.removeFromLeft (roundToInt (currentLayout.valuesYSize));

        paintLabelsX (g, rectLabel);
    }

    // Values X
    if (information.displayGridXValues)
    {
        Rectangle<int> rectValues = rect.removeFromBottom (roundToInt (currentLayout.valuesXSize));

        if (information.strLabelY.isNotEmpty())
            rectValues.removeFromLeft (roundToInt (currentLayout.labelYSize));

        if (information.displayGridYValues)
            rectValues.removeFromLeft (roundToInt (currentLayout.valuesYSize));

        if (canDisplayGrid)
        {
            if (information.isXAxisLogarithmic)
                paintValuesXLog (g, rectValues);
            else
                paintValuesX (g, rectValues);
        }
    }

    // Label Y
    if (information.strLabelY.isNotEmpty())
    {
        Rectangle<int> rectLabel = rect.removeFromLeft (roundToInt (currentLayout.labelYSize));
        paintLabelsY (g, rectLabel);
    }

    // Label Y
    if (information.displayGridYValues)
    {
        Rectangle<int> rectValues = rect.removeFromLeft (roundToInt (currentLayout.valuesYSize));
        
        if (canDisplayGrid)
            paintValuesY (g, rectValues);
    }

    // Background
    g.setColour (currentColourIDs.backgroundColour);
    g.fillRect (rect);

    // Grid X
    if (information.displayGrid && canDisplayGrid)
    {
        if (information.isXAxisLogarithmic)
            paintGridXLog (g, rect);

        else if (information.gridXSize > 0)
            paintGridX (g, rect);
    }
    
    // Grid Y
    if (information.gridYSize > 0 && information.displayGrid && canDisplayGrid)
        paintGridY (g, rect);

    // Data
    if (canDisplayData)
        paintData (g, rect);

    // Outline
    g.setColour (currentColourIDs.outlineColour);
    g.drawRect (rect);
}

//==============================================================================
void PlotComponent::paintTitle (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.setColour (currentColourIDs.textColour);
    g.setFont (currentLayout.fontPlotComponent.withHeight (currentLayout.titleFontSize));
    g.drawFittedText (currentInformation.strTitle, rect, Justification::centred, 2);
}

void PlotComponent::paintValuesX (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.saveState();
    g.addTransform (AffineTransform::translation (rect.getX() + 0.0f, rect.getY() + 0.0f));
    rect.translate (-rect.getX(), -rect.getY());
    
    g.setColour (currentColourIDs.textColour);
    g.setFont (currentLayout.fontPlotComponent.withHeight (currentLayout.valuesFontSize));

    double valueX;
    auto reference = currentInformation.maximumXValue - currentInformation.minimumXValue;

    if (currentInformation.gridXSize > 0 && currentInformation.displayGrid)
    {
        if (currentInformation.gridXMustStartAtZero && currentInformation.minimumXValue <= 0 && currentInformation.maximumXValue >= 0)
        {
            valueX = 0.0;

            while (valueX <= currentInformation.maximumXValue)
            {
                auto pixelX = roundToInt (PlotComponentHelpers::getPixelXPositionForValue (valueX, rect.getWidth(), 
                    currentInformation.minimumXValue, currentInformation.maximumXValue));
                Rectangle<int> newRect = Rectangle<int> (pixelX - roundToInt (currentLayout.valuesLength * 0.5), 0,
                    roundToInt (currentLayout.valuesLength), rect.getHeight());

                String strValue = StringFunctions::getIdealRepresentationForNumber (valueX, reference);
                g.drawFittedText (strValue, newRect, Justification::centred, 1);                

                valueX += currentInformation.gridXSize;
            }

            valueX = 0.0 - currentInformation.gridXSize;

            while (valueX >= currentInformation.minimumXValue - 0.02 * (currentInformation.maximumXValue - currentInformation.minimumXValue))
            {
                auto pixelX = roundToInt (PlotComponentHelpers::getPixelXPositionForValue (valueX, rect.getWidth(), 
                    currentInformation.minimumXValue, currentInformation.maximumXValue));
                Rectangle<int> newRect = Rectangle<int> (pixelX - roundToInt (currentLayout.valuesLength * 0.5), 0,
                    roundToInt (currentLayout.valuesLength), rect.getHeight());

                String strValue = StringFunctions::getIdealRepresentationForNumber (valueX, reference);
                g.drawFittedText (strValue, newRect, Justification::centred, 1);                

                valueX -= currentInformation.gridXSize;
            }
        }
        else
        {
            valueX = currentInformation.minimumXValue;

            while (valueX <= currentInformation.maximumXValue)
            {
                auto pixelX = roundToInt (PlotComponentHelpers::getPixelXPositionForValue (valueX, rect.getWidth(), 
                    currentInformation.minimumXValue, currentInformation.maximumXValue));
                Rectangle<int> newRect = Rectangle<int> (pixelX - roundToInt (currentLayout.valuesLength * 0.5f), 0, 
                    roundToInt (currentLayout.valuesLength), rect.getHeight());

                String strValue = StringFunctions::getIdealRepresentationForNumber (valueX, reference);
                g.drawFittedText (strValue, newRect, Justification::centred, 1);

                valueX += currentInformation.gridXSize;
            }
        }
    }
    else
    {
        valueX = currentInformation.minimumXValue;

        while (valueX <= currentInformation.maximumXValue)
        {
            auto pixelX = roundToInt (PlotComponentHelpers::getPixelXPositionForValue (valueX, rect.getWidth(), 
                currentInformation.minimumXValue, currentInformation.maximumXValue));
            Rectangle<int> newRect = Rectangle<int> (pixelX - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                roundToInt (currentLayout.valuesLength), rect.getHeight());

            String strValue = StringFunctions::getIdealRepresentationForNumber (valueX, reference);
            g.drawFittedText (strValue, newRect, Justification::centred, 1);

            valueX += currentInformation.maximumXValue - currentInformation.minimumXValue;
        }
    }

    g.restoreState();
}

void PlotComponent::paintValuesXLog (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();

    g.saveState();
    g.addTransform (AffineTransform::translation (rect.getX() + 0.f, rect.getY() + 0.f));
    rect.translate (-rect.getX(), -rect.getY());

    g.setColour (currentColourIDs.textColour);
    g.setFont (currentLayout.fontPlotComponent.withHeight (currentLayout.valuesFontSize));

    double valueX;
    double reference = currentInformation.maximumXValue - currentInformation.minimumXValue;

    if (currentInformation.displayGrid)
    {
        valueX = std::pow (10.0, std::ceil (std::log10 (currentInformation.minimumXValue)));

        while (valueX <= currentInformation.maximumXValue)
        {
            auto pixelX = roundToInt (PlotComponentHelpers::getPixelXPositionForValueLog (valueX, rect.getWidth(), 
                currentInformation.minimumXValue, currentInformation.maximumXValue));

            Rectangle<int> newRect = Rectangle<int> (pixelX - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                roundToInt (currentLayout.valuesLength), rect.getHeight());

            String strValue = StringFunctions::getIdealRepresentationForNumber (valueX, reference);
            g.drawFittedText (strValue, newRect, Justification::centred, 1);

            valueX *= 10.0;
        }
    }
    else
    {
        valueX = currentInformation.minimumXValue;

        while (valueX <= currentInformation.maximumXValue)
        {
            auto pixelX = roundToInt (PlotComponentHelpers::getPixelXPositionForValue (valueX, rect.getWidth(), 
                currentInformation.minimumXValue, currentInformation.maximumXValue));
            Rectangle<int> newRect = Rectangle<int> (pixelX - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                roundToInt (currentLayout.valuesLength), rect.getHeight());

            String strValue = StringFunctions::getIdealRepresentationForNumber (valueX, reference);
            g.drawFittedText (strValue, newRect, Justification::centred, 1);

            valueX += currentInformation.maximumXValue - currentInformation.minimumXValue;
        }
    }

    g.restoreState();
}

void PlotComponent::paintValuesY (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.setColour (currentColourIDs.textColour);
    g.setFont (currentLayout.fontPlotComponent.withHeight (currentLayout.valuesFontSize));

    double valueY;
    auto reference = currentInformation.maximumYValue - currentInformation.minimumYValue;
    
    g.saveState();

    if (currentInformation.displayGridYValuesReverted)
    {
        g.setOrigin (rect.getRight(), rect.getY());
        rect.translate (-rect.getX(), -rect.getY());

        g.addTransform (AffineTransform::rotation (-float_Pi * 0.5f));

        Rectangle<int> newRect (rect.getY(), rect.getX(), rect.getHeight(), rect.getWidth());
        newRect.translate (-newRect.getWidth(), -newRect.getHeight());

        g.setOrigin (newRect.getX(), newRect.getY());

        if (currentInformation.gridYSize > 0 && currentInformation.displayGrid)
        {
            if (currentInformation.gridYMustStartAtZero && currentInformation.minimumYValue <= 0 && currentInformation.maximumYValue >= 0)
            {
                valueY = 0.f;

                while (valueY <= currentInformation.maximumYValue)
                {
                    auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, newRect.getWidth(), 
                        currentInformation.minimumYValue, currentInformation.maximumYValue));
                    pixelY = newRect.getWidth() - pixelY;

                    Rectangle<int> newRect2 = Rectangle<int> (pixelY - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                        roundToInt (currentLayout.valuesLength), newRect.getHeight());

                    String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                    g.drawFittedText (strValue, newRect2, Justification::centred, 1);

                    valueY += currentInformation.gridYSize;
                }

                valueY = 0.f - currentInformation.gridYSize;

                while (valueY >= currentInformation.minimumYValue)
                {
                    auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, newRect.getWidth(), 
                        currentInformation.minimumYValue, currentInformation.maximumYValue));
                    pixelY = newRect.getWidth() - pixelY;

                    Rectangle<int> newRect2 = Rectangle<int> (pixelY - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                        roundToInt (currentLayout.valuesLength), newRect.getHeight());

                    String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                    g.drawFittedText (strValue, newRect2, Justification::centred, 1);

                    valueY -= currentInformation.gridYSize;
                }
            }
            else
            {
                valueY = currentInformation.minimumYValue;

                while (valueY <= currentInformation.maximumYValue)
                {
                    auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, newRect.getWidth(), 
                        currentInformation.minimumYValue, currentInformation.maximumYValue));
                    pixelY = newRect.getWidth() - pixelY;

                    Rectangle<int> newRect2 = Rectangle<int> (pixelY - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                        roundToInt (currentLayout.valuesLength), newRect.getHeight());

                    String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                    g.drawFittedText (strValue, newRect2, Justification::centred, 1);

                    valueY += currentInformation.gridYSize;
                }
            }
        }
        else
        {
            valueY = currentInformation.minimumYValue;

            while (valueY <= currentInformation.maximumYValue)
            {
                auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, newRect.getWidth(), 
                    currentInformation.minimumYValue, currentInformation.maximumYValue));
                pixelY = newRect.getWidth() - pixelY;

                Rectangle<int> newRect2 = Rectangle<int> (pixelY - roundToInt (currentLayout.valuesLength * 0.5f), 0,
                    roundToInt (currentLayout.valuesLength), newRect.getHeight());

                String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                g.drawFittedText (strValue, newRect2, Justification::centred, 1);

                valueY += currentInformation.maximumYValue - currentInformation.minimumYValue;
            }
        }
    }
    else
    {
        g.addTransform (AffineTransform::translation (rect.getX() + 0.f, rect.getY() + 0.f));
        rect.translate (-rect.getX(), -rect.getY());

        if (currentInformation.gridYSize > 0 && currentInformation.displayGrid)
        {
            if (currentInformation.gridYMustStartAtZero && currentInformation.minimumYValue <= 0 && currentInformation.maximumYValue >= 0)
            {
                valueY = 0.f;

                while (valueY <= currentInformation.maximumYValue)
                {
                    auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, rect.getHeight(), 
                        currentInformation.minimumYValue, currentInformation.maximumYValue));
                    Rectangle<int> newRect = Rectangle<int> (0, pixelY - 12, rect.getWidth(), 24);

                    newRect.removeFromRight (roundToInt (currentLayout.valuesYMargin));

                    String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                    g.drawFittedText (strValue, newRect, Justification::centredRight, 1);

                    valueY += currentInformation.gridYSize;
                }

                valueY = 0.f - currentInformation.gridYSize;

                while (valueY >= currentInformation.minimumYValue)
                {
                    auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, rect.getHeight(), 
                        currentInformation.minimumYValue, currentInformation.maximumYValue));
                    Rectangle<int> newRect = Rectangle<int> (0, pixelY - 12, rect.getWidth(), 24);

                    newRect.removeFromRight (roundToInt (currentLayout.valuesYMargin));

                    String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                    g.drawFittedText (strValue, newRect, Justification::centredRight, 1);

                    valueY -= currentInformation.gridYSize;
                }
            }
            else
            {
                valueY = currentInformation.minimumYValue;

                while (valueY <= currentInformation.maximumYValue)
                {
                    auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, rect.getHeight(), 
                        currentInformation.minimumYValue, currentInformation.maximumYValue));
                    Rectangle<int> newRect = Rectangle<int> (0, pixelY - 12, rect.getWidth(), 24);

                    newRect.removeFromRight (roundToInt (currentLayout.valuesYMargin));

                    String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                    g.drawFittedText (strValue, newRect, Justification::centredRight, 1);

                    valueY += currentInformation.gridYSize;
                }
            }
        }
        else
        {
            valueY = currentInformation.minimumXValue;

            while (valueY <= currentInformation.maximumYValue)
            {
                auto pixelY = roundToInt (PlotComponentHelpers::getPixelYPositionForValue (valueY, rect.getHeight(), 
                    currentInformation.minimumYValue, currentInformation.maximumYValue));
                Rectangle<int> newRect = Rectangle<int> (0, pixelY - 12, rect.getWidth(), 24);

                newRect.removeFromRight (roundToInt (currentLayout.valuesYMargin));

                String strValue = StringFunctions::getIdealRepresentationForNumber (valueY, reference);
                g.drawFittedText (strValue, newRect, Justification::centredRight, 1);

                valueY += currentInformation.maximumYValue - currentInformation.minimumYValue;
            }
        }
    }

    g.restoreState();
}

void PlotComponent::paintLabelsX (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.setColour (currentColourIDs.textColour);
    g.setFont (currentLayout.fontPlotComponent.withHeight (currentLayout.labelFontSize));
    g.drawFittedText (currentInformation.strLabelX, rect, Justification::centred, 2);
}

void PlotComponent::paintLabelsY (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.saveState();
    
    g.setOrigin (rect.getRight(), rect.getY());
    rect.translate (-rect.getX(), -rect.getY());

    g.addTransform (AffineTransform::rotation (-float_Pi * 0.5f));

    Rectangle<int> newRect (rect.getY(), rect.getX(), rect.getHeight(), rect.getWidth());
    newRect.translate (-newRect.getWidth(), -newRect.getHeight());

    g.setColour (currentColourIDs.textColour);
    g.setFont (currentLayout.fontPlotComponent.withHeight (currentLayout.labelFontSize));
    g.drawFittedText (currentInformation.strLabelY, newRect, Justification::centred, 2);

    g.restoreState();
}

void PlotComponent::paintGridX (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.saveState();
    g.addTransform (AffineTransform::translation (rect.getX() + 0.f, rect.getY() + 0.f));
    rect.translate (-rect.getX(), -rect.getY());
    
    double valueX, pixelX;
        
    if (currentInformation.gridXMustStartAtZero && currentInformation.minimumXValue <= 0 && currentInformation.maximumXValue >= 0)
    {
        valueX = 0.0;

        while (valueX <= currentInformation.maximumXValue)
        {
            pixelX = PlotComponentHelpers::getIntegerPixelXPositionForValue (valueX, rect.getWidth(), 
                currentInformation.minimumXValue, currentInformation.maximumXValue);

            g.setColour (Colours::lightgrey);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, rect.getHeight() + 0.f);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, currentLayout.markerSize);
            g.drawLine ((float)pixelX, rect.getHeight() - currentLayout.markerSize, (float)pixelX, rect.getHeight() + 0.f);

            valueX += currentInformation.gridXSize;
        }

        valueX = 0.0 - currentInformation.gridXSize;

        while (valueX >= currentInformation.minimumXValue)
        {
            pixelX = PlotComponentHelpers::getIntegerPixelXPositionForValue (valueX, rect.getWidth(), 
                currentInformation.minimumXValue, currentInformation.maximumXValue);

            g.setColour (Colours::lightgrey);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, rect.getHeight() + 0.f);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, currentLayout.markerSize);
            g.drawLine ((float)pixelX, rect.getHeight() - currentLayout.markerSize, (float)pixelX, rect.getHeight() + 0.f);

            valueX -= currentInformation.gridXSize;
        }
    }
    else
    {
        valueX = currentInformation.minimumXValue;

        while (valueX <= currentInformation.maximumXValue)
        {
            pixelX = PlotComponentHelpers::getIntegerPixelXPositionForValue (valueX, rect.getWidth(), 
                currentInformation.minimumXValue, currentInformation.maximumXValue);

            g.setColour (currentColourIDs.gridColour);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, rect.getHeight() + 0.f);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, currentLayout.markerSize);
            g.drawLine ((float)pixelX, rect.getHeight() - currentLayout.markerSize, (float)pixelX, rect.getHeight() + 0.f);

            valueX += currentInformation.gridXSize;
        }
    }

    g.restoreState();
}

void PlotComponent::paintGridXLog (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();

    g.saveState();
    g.addTransform (AffineTransform::translation (rect.getX() + 0.f, rect.getY() + 0.f));
    rect.translate (-rect.getX(), -rect.getY());

    auto numOctaves = std::log10 (currentInformation.maximumXValue) - std::log10 (currentInformation.minimumXValue);
    auto valueX = std::pow (10.0, std::floor (std::log10 (currentInformation.minimumXValue)));

    if (numOctaves <= currentInformation.numOctavesMaxFullGridLog)
    {
        while (valueX <= currentInformation.maximumXValue)
        {
            for (auto i = 1; i <= 9; i++)
            {
                if (valueX * i >= currentInformation.minimumXValue && valueX * i <= currentInformation.maximumXValue)
                {
                    auto pixelX = PlotComponentHelpers::getIntegerPixelXPositionForValueLog (valueX * i, rect.getWidth(),
                        currentInformation.minimumXValue, currentInformation.maximumXValue);

                    g.setColour (currentColourIDs.gridColour);
                    g.drawLine ((float)pixelX, 0.f, (float)pixelX, rect.getHeight() + 0.f);

                    g.setColour (currentColourIDs.markerColour);
                    g.drawLine ((float)pixelX, 0.f, (float)pixelX, currentLayout.markerSize);
                    g.drawLine ((float)pixelX, rect.getHeight() - currentLayout.markerSize, (float)pixelX, rect.getHeight() + 0.f);
                }
            }

            valueX *= 10.f;
        }
    }
    else
    {
        while (valueX <= currentInformation.maximumXValue)
        {
            auto pixelX = PlotComponentHelpers::getIntegerPixelXPositionForValueLog (valueX, rect.getWidth(),
                currentInformation.minimumXValue, currentInformation.maximumXValue);

            g.setColour (currentColourIDs.gridColour);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, rect.getHeight() + 0.f);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine ((float)pixelX, 0.f, (float)pixelX, currentLayout.markerSize);
            g.drawLine ((float)pixelX, rect.getHeight() - currentLayout.markerSize, (float)pixelX, rect.getHeight() + 0.f);
            
            valueX *= 10.f;
        }
    }

    g.restoreState();
}

void PlotComponent::paintGridY (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.saveState();
    g.addTransform (AffineTransform::translation (rect.getX() + 0.f, rect.getY() + 0.f));
    rect.translate (-rect.getX(), -rect.getY());
    
    double valueY, pixelY;

    if (currentInformation.gridYMustStartAtZero && currentInformation.minimumYValue <= 0 && currentInformation.maximumYValue >= 0)
    {
        valueY = 0.0;

        while (valueY <= currentInformation.maximumYValue)
        {
            pixelY = PlotComponentHelpers::getIntegerPixelYPositionForValue (valueY, rect.getHeight(), 
                currentInformation.minimumYValue, currentInformation.maximumYValue);

            g.setColour (currentColourIDs.gridColour);
            g.drawLine (0.f, (float)pixelY, rect.getWidth() + 0.f, (float)pixelY);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine (0.f, (float)pixelY, currentLayout.markerSize, (float)pixelY);
            g.drawLine (rect.getWidth() + 0.f, (float)pixelY, rect.getWidth() - currentLayout.markerSize, (float)pixelY);

            valueY += currentInformation.gridYSize;
        }

        valueY = 0.0 - currentInformation.gridYSize;

        while (valueY >= currentInformation.minimumYValue)
        {
            pixelY = PlotComponentHelpers::getIntegerPixelYPositionForValue (valueY, rect.getHeight(), 
                currentInformation.minimumYValue, currentInformation.maximumYValue);

            g.setColour (currentColourIDs.gridColour);
            g.drawLine (0.f, (float)pixelY, rect.getWidth() + 0.f, (float)pixelY);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine (0.f, (float)pixelY, currentLayout.markerSize, (float)pixelY);
            g.drawLine (rect.getWidth() + 0.f, (float)pixelY, rect.getWidth() - currentLayout.markerSize, (float)pixelY);

            valueY -= currentInformation.gridYSize;
        }
    }
    else
    {
        valueY = currentInformation.minimumYValue;

        while (valueY <= currentInformation.maximumYValue)
        {
            pixelY = PlotComponentHelpers::getIntegerPixelYPositionForValue (valueY, rect.getHeight(), 
                currentInformation.minimumYValue, currentInformation.maximumYValue);

            g.setColour (currentColourIDs.gridColour);
            g.drawLine (0.f, (float)pixelY, rect.getWidth() + 0.f, (float)pixelY);

            g.setColour (currentColourIDs.markerColour);
            g.drawLine (0.f, (float)pixelY, currentLayout.markerSize, (float)pixelY);
            g.drawLine (rect.getWidth() + 0.f, (float)pixelY, rect.getWidth() - currentLayout.markerSize, (float)pixelY);

            valueY += currentInformation.gridYSize;
        }        
    }

    g.restoreState();
}

void PlotComponent::paintData (Graphics& g, Rectangle<int> rect)
{
    auto currentInformation = getInformation();
    auto currentColourIDs = getColourIDs();
    auto currentLayout = getPlotLayout();
    
    g.saveState();

    g.excludeClipRegion (Rectangle<int> (0, 0, getWidth(), rect.getY()));
    g.excludeClipRegion (Rectangle<int> (0, rect.getBottom(), getWidth(), getHeight() - rect.getBottom()));
    g.excludeClipRegion (Rectangle<int> (0, 0, rect.getX(), getHeight()));
    g.excludeClipRegion (Rectangle<int> (rect.getRight(), 0, getWidth() - rect.getRight(), getHeight()));
    
    g.addTransform (AffineTransform::translation (rect.getX() + 0.f, rect.getY() + 0.f));
    rect.translate (-rect.getX(), -rect.getY());

    auto numPoints = dataStructure.numPoints.load();
    auto numEntries = dataStructure.numEntries.load();
    
    const ScopedLock myScopedLock (dataLock);
    
    for (auto n = 0; n < numEntries; n++)
    {
        auto indexColour = n % currentColourIDs.dataColours.size();
        g.setColour (currentColourIDs.dataColours[indexColour]);

        Path thePath;
        bool mustStartNewPath = true;

        for (auto i = 0; i < numPoints; i++)
        {
            auto valueX = dataStructure.dataX[i];
            auto valueY = dataStructure.dataY[n][i];

            if (isnan (valueY))
            {
                mustStartNewPath = true;
            }
            else
            {
                if (isinf (valueY))
                {
                    if (valueY == -std::numeric_limits<double>::infinity())
                        valueY = (float) (currentInformation.minimumYValue - 10 * (currentInformation.maximumYValue - currentInformation.minimumYValue));
                    else
                        valueY = (float) (currentInformation.maximumYValue + 10 * (currentInformation.maximumYValue - currentInformation.minimumYValue));
                }
                
                double pixelX;

                if (currentInformation.isXAxisLogarithmic)
                    pixelX = PlotComponentHelpers::getPixelXPositionForValueLog (valueX, rect.getWidth(), 
                        currentInformation.minimumXValue, currentInformation.maximumXValue);
                else
                    pixelX = PlotComponentHelpers::getPixelXPositionForValue (valueX, rect.getWidth(),
                        currentInformation.minimumXValue, currentInformation.maximumXValue);

                auto pixelY = PlotComponentHelpers::getPixelYPositionForValue (valueY, rect.getHeight(), 
                    currentInformation.minimumYValue, currentInformation.maximumYValue);

                if (mustStartNewPath)
                {
                    thePath.startNewSubPath ((float)pixelX, (float)pixelY);
                    mustStartNewPath = false;
                }
                else
                    thePath.lineTo ((float)pixelX, (float)pixelY);
            }
        }

        PathStrokeType theStrokeType = PathStrokeType (1.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded);
        g.strokePath (thePath, theStrokeType);
    }

    g.restoreState();
}
