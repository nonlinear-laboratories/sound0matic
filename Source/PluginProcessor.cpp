// PluginProcessor.cpp

#include "PluginProcessor.h"
#include "PluginEditor.h"

Sound0maticProcessor::Sound0maticProcessor()
    : AudioProcessor(
          BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

Sound0maticProcessor::~Sound0maticProcessor()
{
}

const juce::String Sound0maticProcessor::getName() const
{
     return "sound0matic";
}

bool Sound0maticProcessor::acceptsMidi() const
{
     return false;
}
bool Sound0maticProcessor::producesMidi() const
{
     return false;
}
bool Sound0maticProcessor::isMidiEffect() const
{
     return false;
}
double Sound0maticProcessor::getTailLengthSeconds() const
{
     return 0.0;
}

int Sound0maticProcessor::getNumPrograms()
{
     return 1;
}
int Sound0maticProcessor::getCurrentProgram()
{
     return 0;
}
void Sound0maticProcessor::setCurrentProgram(int)
{
}
const juce::String Sound0maticProcessor::getProgramName(int)
{
     return {};
}
void Sound0maticProcessor::changeProgramName(int, const juce::String &)
{
}

void Sound0maticProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
     juce::File sampleFile("/home/null/dev/sound0matic/Resource/cymbalom-a3.wav");
     sampleLoader.loadSampleFromFile(sampleFile, sampleRate);
     sampleLoader.setTrimRange(0.0f, 1.0f);
     playbackPosition = 0;
     fftProcessor.prepare(1024, 512);
     // spectralFX.prepare(sampleRate, samplesPerBlock);
     // postFX.prepare(sampleRate, samplesPerBlock);
}

void Sound0maticProcessor::releaseResources()
{
}

bool Sound0maticProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
     return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void Sound0maticProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &)
{
     // Zero out output buffer
     buffer.clear();

     // Prepare working buffer
     workingBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
     workingBuffer.clear();

     // Mono read pointer from sample
     const auto &source = sampleLoader.getBuffer();
     int availableSamples = sampleLoader.getNumSamples();

     for (int i = 0; i < buffer.getNumSamples(); ++i)
     {
          float sample = 0.0f;

          if (playbackPosition < availableSamples)
               sample = source.getReadPointer(0)[playbackPosition++];

          fftProcessor.pushSample(sample);

          if (fftProcessor.readyToProcess())
          {
               fftProcessor.performSTFT();

               // Process bins here, e.g.:
               // spectralFX.processBins(fftProcessor.real, fftProcessor.imag);

               fftProcessor.performISTFT();
          }

          float outSample = fftProcessor.popSample();

          for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
               buffer.setSample(ch, i, outSample);
     }
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
     return new Sound0maticProcessor();
}

bool Sound0maticProcessor::hasEditor() const
{
     return false;
}
juce::AudioProcessorEditor *Sound0maticProcessor::createEditor()
{
     return nullptr;
}

void Sound0maticProcessor::getStateInformation(juce::MemoryBlock &)
{
}
void Sound0maticProcessor::setStateInformation(const void *, int)
{
}
