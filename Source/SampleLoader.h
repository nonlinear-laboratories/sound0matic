// SampleLoader.h

#pragma once

#include <JuceHeader.h>

class SampleLoader
{
   public:
     SampleLoader();
     ~SampleLoader();

     bool loadSampleFromFile(const juce::File &file, double targetSampleRate);
     void setTrimRange(float start, float end); // normalized 0.0 to 1.0

     const juce::AudioBuffer<float> &getBuffer() const;
     int getNumSamples() const;

   private:
     juce::AudioBuffer<float> buffer;
     float trimStart = 0.0f;
     float trimEnd = 1.0f;

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleLoader)
};
