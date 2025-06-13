// SampleLoader.h
#pragma once

#include "BufferAudioFormatReader.h"
#include <JuceHeader.h>

class SampleLoader
{
   public:
     SampleLoader();
     ~SampleLoader();

     bool loadSampleFromFile(const juce::File &file, double targetSampleRate);
     void setTrimRange(float start, float end);
     const juce::AudioBuffer<float> &getBuffer() const;
     int getNumSamples() const;

     void setupSampler(juce::Synthesiser &synth, double sampleRate);

   private:
     juce::AudioBuffer<float> buffer;
     float trimStart = 0.0f;
     float trimEnd = 1.0f;
     std::unique_ptr<juce::AudioFormatReader> internalReader;
};