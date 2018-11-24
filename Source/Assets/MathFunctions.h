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