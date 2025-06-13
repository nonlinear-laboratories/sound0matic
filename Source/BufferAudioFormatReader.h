// BufferAudioFormatReader.h

#pragma once
#include <JuceHeader.h>

class BufferAudioFormatReader : public juce::AudioFormatReader
{
   public:
     BufferAudioFormatReader(const juce::AudioBuffer<float> &sourceBuffer,
                             double sampleRate);

     bool readSamples(int *const *destSamples, int numDestChannels,
                      int startOffsetInDestBuffer, int64 startSampleInFile,
                      int numSamples) override;

   private:
     const juce::AudioBuffer<float> &buffer;
};
