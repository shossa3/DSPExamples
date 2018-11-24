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
/**
    A class that provides multiple functions related with Strings for 
    visualization of data.
*/
struct StringFunctions
{
    //==============================================================================
    /** This function returns an ideal representation for any given number in a
        String which can be useful in visualisation of data components.
    */
    template<typename FloatType>
    static const String getIdealRepresentationForNumber (FloatType value, FloatType reference)
    {
        auto intNumber = roundToInt (std::floor (value));
        auto fracNumber = value - intNumber;

        if (fracNumber == (FloatType) 0.0)
            return String (roundToInt (value));

        else if (std::abs (1 - intNumber / value) < (FloatType) 1e-5)
            return String (roundToInt (value));

        else if (std::abs (value) * 100 < std::abs (reference) && std::abs (value) < 1 && reference != -1)
            return "0";

        else if (std::abs (value) <= (FloatType) 1e-5)
        {
            String strResult = String (value).replace ("e0", "e").replace ("e-0", "e-");
            return strResult;
        }

        else
            return String (value, 6).trimCharactersAtEnd ("0").trimCharactersAtEnd (".");
    }

    //==============================================================================
    /** This function returns a double value from a String containing both a value
        and a character for a given unit, using LTSpice syntax.
    */
    static const double getValueFromStringWithUnits (String strValue)
    {
        String strNumber, strUnit;
        strValue = strValue.toLowerCase();

        strNumber = strValue.retainCharacters ("0123456789.-");
        strUnit = strValue.retainCharacters ("abcdefghijklmnopqrstuvwxyz");

        auto result = strNumber.getDoubleValue();

        if (strUnit.compare ("k") == 0) result *= 1e3;
        else if (strUnit.compare ("meg") == 0) result *= 1e6;
        else if (strUnit.compare ("g") == 0) result *= 1e9;
        else if (strUnit.compare ("t") == 0) result *= 1e12;
        else if (strUnit.compare ("m") == 0) result *= 1e-3;
        else if (strUnit.compare ("u") == 0) result *= 1e-6;
        else if (strUnit.compare ("n") == 0) result *= 1e-9;
        else if (strUnit.compare ("p") == 0) result *= 1e-12;
        else if (strUnit.compare ("f") == 0) result *= 1e-15;

        return result;
    }

    //==============================================================================
    /** This function returns a String from a given double variable using unit 
        characters with LTSpice syntax.
    */
    static const String getStringWithUnitsFromValue (double value, int numDecimals = 7)
    {
        String strResult;

        if (numDecimals <= 0)
        {
            if (std::abs (value) >= 1e12) strResult = String (roundToInt (value / 1e12));
            else if (std::abs (value) >= 1e9) strResult = String (roundToInt (value / 1e9));
            else if (std::abs (value) >= 1e6) strResult = String (roundToInt (value / 1e6));
            else if (std::abs (value) >= 1e3) strResult = String (roundToInt (value / 1e3));
            else if (std::abs (value) >= 1e0) strResult = String (roundToInt (value / 1e0));
            else if (std::abs (value) >= 1e-3) strResult = String (roundToInt (value / 1e-3));
            else if (std::abs (value) >= 1e-6) strResult = String (roundToInt (value / 1e-6));
            else if (std::abs (value) >= 1e-9) strResult = String (roundToInt (value / 1e-9));
            else if (std::abs (value) >= 1e-12) strResult = String (roundToInt (value / 1e-12));
            else if (std::abs (value) >= 1e-15) strResult = String (roundToInt (value / 1e-15));
        }
        else
        {
            auto valueUnit = 1.0;

            if (std::abs (value) >= 1e12) valueUnit = value / 1e12;
            else if (std::abs (value) >= 1e9) valueUnit = value / 1e9;
            else if (std::abs (value) >= 1e6) valueUnit = value / 1e6;
            else if (std::abs (value) >= 1e3) valueUnit = value / 1e3;
            else if (std::abs (value) >= 1e0) valueUnit = value / 1e0;
            else if (std::abs (value) >= 1e-3) valueUnit = value / 1e-3;
            else if (std::abs (value) >= 1e-6) valueUnit = value / 1e-6;
            else if (std::abs (value) >= 1e-9) valueUnit = value / 1e-9;
            else if (std::abs (value) >= 1e-12) valueUnit = value / 1e-12;
            else if (std::abs (value) >= 1e-15) valueUnit = value / 1e-15;

            strResult = String (valueUnit, numDecimals);
        }

        if (strResult.indexOf (".") != -1)
        {
            strResult = strResult.trimCharactersAtEnd ("0");
            strResult = strResult.trimCharactersAtEnd (".");
        }

        if (std::abs (value) >= 1e12) strResult += "T";
        else if (std::abs (value) >= 1e9) strResult += "G";
        else if (std::abs (value) >= 1e6) strResult += "Meg";
        else if (std::abs (value) >= 1e3) strResult += "k";
        else if (std::abs (value) >= 1e0) strResult += "";
        else if (std::abs (value) >= 1e-3) strResult += "m";
        else if (std::abs (value) >= 1e-6) strResult += "u";
        else if (std::abs (value) >= 1e-9) strResult += "n";
        else if (std::abs (value) >= 1e-12) strResult += "p";
        else if (std::abs (value) >= 1e-15) strResult += "f";

        return strResult;
    }
};
