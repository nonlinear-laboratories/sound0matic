// PluginProcessor.cpp

#include "PluginProcessor.h"
#include "PluginEditor.h"
#define JUCE_DEBUG 1

Sound0maticProcessor::Sound0maticProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
     // ...Debugging
     DBG("Sound0maticProcessor::constructor");

     // Register parameter listeners
     parameters.addParameterListener("sinGain", this);
     parameters.addParameterListener("transGain", this);
     parameters.addParameterListener("resGain", this);
     parameters.addParameterListener("pitchShift", this);
     parameters.addParameterListener("timeStretch", this);
     parameters.addParameterListener("bypassSpectral", this);
     parameters.addParameterListener("bypassPhase", this);
}

Sound0maticProcessor::~Sound0maticProcessor()
{
}

const juce::String Sound0maticProcessor::getName() const
{
     return "sound0matic";
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

void Sound0maticProcessor::parameterChanged(const juce::String &id, float newValue)
{
     DBG("Parameter changed: " << id << " = " << newValue);
     // update vocoder or STN module here if needed
}

void Sound0maticProcessor::changeProgramName(int, const juce::String &)
{
}

void Sound0maticProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
     DBG("prepareToPlay called");

     // Load sample and initialize synthesizer
     sampleLoader.loadSampleFromFile(
         juce::File("/home/null/dev/sound0matic/Resource/cymbalom-a3.wav"), sampleRate);
     sampleLoader.setupSampler(synth, sampleRate);

     // Other module prep if needed
     // fftProcessor.prepare(...);
     // vocoder.prepare(...);
     // spectralFX.prepare(...);
     // etc.

     synth.setCurrentPlaybackSampleRate(sampleRate);
}

void Sound0maticProcessor::releaseResources()
{
}

bool Sound0maticProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
     return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void Sound0maticProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                        juce::MidiBuffer &midiMessages)
{
     for (const auto metadata : midiMessages)
     {
          auto msg = metadata.getMessage();
          DBG("MIDI: " << msg.getDescription());
     }

     juce::ScopedNoDenormals noDenormals;
     auto totalNumInputChannels = getTotalNumInputChannels();
     auto totalNumOutputChannels = getTotalNumOutputChannels();

     // Clear unused output channels
     for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
          buffer.clear(i, 0, buffer.getNumSamples());

     buffer.clear(); // Important: clear before synth fills it

     // MIDI → Synth → AudioBuffer
     synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
     return new Sound0maticProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout
Sound0maticProcessor::createParameterLayout()
{
     std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

     // STN Gain Parameters
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "sinGain", "Sinusoidal Gain", juce::NormalisableRange<float>(0.0f, 1.0f),
         1.0f));
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "transGain", "Transient Gain", juce::NormalisableRange<float>(0.0f, 1.0f),
         1.0f));
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "resGain", "Residual Gain", juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));

     // vocoder parameters
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "pitchShift", "Pitch Shift", juce::NormalisableRange<float>(0.5f, 2.0f),
         1.0f));
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "timeStretch", "Time Stretch", juce::NormalisableRange<float>(0.5f, 2.0f),
         1.0f));

     // FX Chain Parameters
     params.push_back(std::make_unique<juce::AudioParameterBool>(
         "bypassSpectral", "Bypass SpectralFX", false));
     params.push_back(std::make_unique<juce::AudioParameterBool>(
         "bypassPhase", "Bypass PhaseFX", false));

     return {params.begin(), params.end()};
}

bool Sound0maticProcessor::hasEditor() const
{
     return false;
}
juce::AudioProcessorEditor *Sound0maticProcessor::createEditor()
{
     return new Sound0maticEditor(*this);
}

void Sound0maticProcessor::getStateInformation(juce::MemoryBlock &)
{
}
void Sound0maticProcessor::setStateInformation(const void *, int)
{
}
