// BufferAudioFormatReader.cpp

#include "BufferAudioFormatReader.h"

BufferAudioFormatReader::BufferAudioFormatReader(
    const juce::AudioBuffer<float> &sourceBuffer, double sampleRate)
    : juce::AudioFormatReader(nullptr, "MemoryBuffer"), buffer(sourceBuffer)
{
     this->sampleRate = sampleRate;
     lengthInSamples = buffer.getNumSamples();
     numChannels = buffer.getNumChannels();
     bitsPerSample = 32;
     usesFloatingPointData = true;
}

bool BufferAudioFormatReader::readSamples(int *const *destSamples, int numDestChannels,
                                          int startOffsetInDestBuffer,
                                          int64 startSampleInFile, int numSamples)
{
     if (startSampleInFile < 0 ||
         startSampleInFile + numSamples > buffer.getNumSamples())
          return false;

     const int channelsToCopy = std::min(numDestChannels, buffer.getNumChannels());

     for (int ch = 0; ch < channelsToCopy; ++ch)
     {
          const float *src = buffer.getReadPointer(ch, (int)startSampleInFile);
          float *dest =
              reinterpret_cast<float *>(destSamples[ch]) + startOffsetInDestBuffer;

          std::memcpy(dest, src, sizeof(float) * numSamples);
     }

     return true;
}
