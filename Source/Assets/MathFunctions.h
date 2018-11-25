/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/** Remaps a normalised value (between 0 and 1) to a target range, with a logarithmic mapping.
    The target range must be strictly positive.
*/
template <typename Type>
const Type jmaplintolog (Type value0To1, Type targetRangeMin, Type targetRangeMax)
{
    jassert (targetRangeMin > 0);
    jassert (targetRangeMax > 0);

    return std::pow ((Type) 10.0, value0To1 * std::log10 (targetRangeMax) + ((Type) 1.0 - value0To1) * std::log10 (targetRangeMin));
}

/** Remaps a logarithmically mapped value to a linear normalised value (between 0 and 1). */
template <typename Type>
const Type jmaplogtolin (Type valueLog, Type targetRangeMin, Type targetRangeMax)
{
    jassert (targetRangeMin > 0);
    jassert (targetRangeMax > 0);

    return (std::log10 (valueLog) - std::log10 (targetRangeMin)) / (std::log10 (targetRangeMax) - std::log10 (targetRangeMin));
}
