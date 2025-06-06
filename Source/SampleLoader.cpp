#include "SampleLoader.h"

SampleLoader::SampleLoader()
{
}
SampleLoader::~SampleLoader()
{
}

bool SampleLoader::loadSampleFromFile(const juce::File &file, double targetSampleRate)
{
     juce::AudioFormatManager formatManager;
     formatManager.registerBasicFormats();

     std::unique_ptr<juce::AudioFormatReader> reader(
         formatManager.createReaderFor(file));
     if (reader == nullptr)
          return false;

     auto totalSamples = static_cast<int>(reader->lengthInSamples);
     juce::AudioBuffer<float> tempBuffer((int)reader->numChannels, totalSamples);
     reader->read(&tempBuffer, 0, totalSamples, 0, true, true);

     // Convert to mono if needed
     if (reader->numChannels > 1)
     {
          buffer.setSize(1, totalSamples);
          buffer.clear();
          for (int ch = 0; ch < reader->numChannels; ++ch)
               buffer.addFrom(0, 0, tempBuffer, ch, 0, totalSamples,
                              1.0f / reader->numChannels);
     }
     else
     {
          buffer = std::move(tempBuffer);
     }

     // Resample if needed
     if (reader->sampleRate != targetSampleRate)
     {
          juce::AudioFormatReader *rawReader = formatManager.createReaderFor(file);
          juce::AudioFormatReaderSource readerSource(rawReader, true);

          juce::ResamplingAudioSource resampler(&readerSource, false, 1);
          resampler.setResamplingRatio(reader->sampleRate / targetSampleRate);
          resampler.prepareToPlay(buffer.getNumSamples(), targetSampleRate);

          juce::AudioBuffer<float> resampledBuffer(
              1,
              (int)(buffer.getNumSamples() * (targetSampleRate / reader->sampleRate)));
          juce::AudioSourceChannelInfo info(&resampledBuffer, 0,
                                            resampledBuffer.getNumSamples());
          resampler.getNextAudioBlock(info);
          resampler.releaseResources();

          buffer = std::move(resampledBuffer);
     }

     return true;
}

void SampleLoader::setTrimRange(float start, float end)
{
     trimStart = juce::jlimit(0.0f, 1.0f, start);
     trimEnd = juce::jlimit(trimStart, 1.0f, end);
}

const juce::AudioBuffer<float> &SampleLoader::getBuffer() const
{
     return buffer;
}

int SampleLoader::getNumSamples() const
{
     int total = buffer.getNumSamples();
     int startSample = static_cast<int>(trimStart * total);
     int endSample = static_cast<int>(trimEnd * total);
     return endSample - startSample;
}
