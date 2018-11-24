/*
  ==============================================================================

  Shaikat Hossain DSPExamples

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/**
    Class with static functions to access to all the external data
*/
struct FileSystem
{
public:
    //==============================================================================
    static const File getDataFolder()
    {
        File fileExecutable = File::getSpecialLocation (File::SpecialLocationType::currentApplicationFile);

        if (fileExecutable.getSiblingFile ("./Data/").isDirectory())
            return fileExecutable.getSiblingFile ("./Data/");

        else if (fileExecutable.getSiblingFile ("../Data/").isDirectory())
            return fileExecutable.getSiblingFile ("../Data/");

        else if (fileExecutable.getSiblingFile ("../../Data/").isDirectory())
            return fileExecutable.getSiblingFile ("../../Data/");

        else if (fileExecutable.getSiblingFile ("../../../Data/").isDirectory())
            return fileExecutable.getSiblingFile ("../../../Data/");

        else if (fileExecutable.getSiblingFile ("../../../../Data/").isDirectory())
            return fileExecutable.getSiblingFile ("../../../../Data/");

        else if (fileExecutable.getSiblingFile ("../../../../../Data/").isDirectory())
            return fileExecutable.getSiblingFile ("../../../../../Data/");

        else if (fileExecutable.getSiblingFile ("../../../../../../Data/").isDirectory())
            return fileExecutable.getSiblingFile ("../../../../../../Data/");

        else if (fileExecutable.getSiblingFile("../../../../../../../Data/").isDirectory())
            return fileExecutable.getSiblingFile("../../../../../../../Data/");

        else
            return File();
    }

    //==============================================================================
    static const File getAudioFolder()
    {
        if (getDataFolder() == File())
            return File();
        else
            return getDataFolder().getChildFile ("./Audio/");
    }
    
    static const Array<File> getAudioList()
    {
        File folder = getAudioFolder();
        Array <File> results;

        if (folder != File())
            folder.findChildFiles (results, File::findFiles, true, "*.wav;*.aif;*.aiff;*.mp3");

        return results;
    }
};

