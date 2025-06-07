// PluginProcessor.cpp

#include "PluginProcessor.h"
#include "PluginEditor.h"

Sound0maticProcessor::Sound0maticProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
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
     vocoder.prepare(sampleRate, 1024, 512);
     phaseFX.prepare(sampleRate, samplesPerBlock);
     postFX.prepare(sampleRate, samplesPerBlock);
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
     float pitch = *parameters.getRawParameterValue("pitchShift");
     float stretch = *parameters.getRawParameterValue("timeStretch");
     bool bypassSpectral = parameters.getRawParameterValue("bypassSpectral")->load();
     bool bypassPhase = parameters.getRawParameterValue("bypassPhase")->load();
     // Apply parameters
     vocoder.setPitchShiftRatio(pitch);
     vocoder.setTimeStretchRatio(stretch);
     spectralFX.setBypass(bypassSpectral);
     phaseFX.setBypass(bypassPhase);
     buffer.clear();
     workingBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples());
     workingBuffer.clear();
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
               // --- STN Analysis (Mask-based Routing Logic) ---
               juce::AudioBuffer<float> magnitude;
               magnitude.setSize(fftProcessor.real.getNumChannels(),
                                 fftProcessor.real.getNumSamples());
               for (int ch = 0; ch < magnitude.getNumChannels(); ++ch)
               {
                    const float *realData = fftProcessor.real.getReadPointer(ch);
                    const float *imagData = fftProcessor.imag.getReadPointer(ch);
                    float *magData = magnitude.getWritePointer(ch);

                    for (int j = 0; j < magnitude.getNumSamples(); ++j)
                         magData[j] = std::sqrt(realData[j] * realData[j] +
                                                imagData[j] * imagData[j]);
               }

               stnModule.analyze(magnitude);
               auto &sMask = stnModule.getSinusoidMask();
               auto &tMask = stnModule.getTransientMask();
               auto &rMask = stnModule.getResidualMask();
               // (Optional) apply masks to fftProcessor.real/imag
               // TODO: routing / selective FX
               vocoder.process(fftProcessor.real, fftProcessor.imag);
               // --- FX Chain ---
               spectralFX.processBins(fftProcessor.real, fftProcessor.imag);
               phaseFX.process(fftProcessor.real, fftProcessor.imag);
               fftProcessor.performISTFT();
          }

          float outSample = fftProcessor.popSample();
          for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
               buffer.setSample(ch, i, outSample);
     }

     // Final gain or output shaping
     postFX.applyGain(buffer);
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
     return new Sound0maticProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout
Sound0maticProcessor::createParameterLayout()
{
     std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "pitchShift", "Pitch Shift", juce::NormalisableRange<float>(0.5f, 2.0f),
         1.0f));
     params.push_back(std::make_unique<juce::AudioParameterFloat>(
         "timeStretch", "Time Stretch", juce::NormalisableRange<float>(0.5f, 2.0f),
         1.0f));
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
