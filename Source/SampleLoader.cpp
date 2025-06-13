// SampleLoader.cpp

#include "SampleLoader.h"
#include "BufferAudioFormatReader.h"

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

     // Force mono
     buffer.setSize(1, totalSamples);
     buffer.clear();
     for (int ch = 0; ch < reader->numChannels; ++ch)
          buffer.addFrom(0, 0, tempBuffer, ch, 0, totalSamples,
                         1.0f / reader->numChannels);

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

void SampleLoader::setupSampler(juce::Synthesiser &synth, double sampleRate)
{
     synth.clearVoices();
     synth.clearSounds();

     synth.addVoice(new juce::SamplerVoice());

     if (buffer.getNumSamples() == 0)
          return;

     juce::BigInteger allNotes;
     allNotes.setRange(0, 128, true);

     auto reader = std::make_unique<BufferAudioFormatReader>(buffer, sampleRate);
     auto *sound =
         new juce::SamplerSound("sample", *reader, allNotes, 60, 0.0, 0.1, 10.0);
     synth.addSound(sound);

     DBG("Voices count: " << synth.getNumVoices());
     DBG("Sounds count: " << synth.getNumSounds());
}
