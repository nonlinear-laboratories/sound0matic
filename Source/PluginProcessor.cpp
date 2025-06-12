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

void Sound0maticProcessor::handleMidiPitch(int midiNote)
{
     currentMidiNote = midiNote;
     float semitoneRatio = std::pow(2.0f, (midiNote - 60) / 12.0f);
     vocoder.setPitchShiftRatio(semitoneRatio);
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

     // ...Debugging
     DBG("prepareToPlay: entered");

     juce::File sampleFile("/home/null/dev/sound0matic/Resource/cymbalom-a3.wav");
     // ...Debugging
     DBG("sampleLoader loading file");

     sampleLoader.loadSampleFromFile(sampleFile, sampleRate);
     //...Debugging
     DBG("sampleLoader file loaded");
     sampleLoader.setTrimRange(0.0f, 1.0f);
     playbackPosition = 0;

     // ...Debugging
     DBG("fftProcessor ready");
     fftProcessor.prepare(1024, 512); // Placeholder value for blockSize and hopSize

     vocoder.prepare(sampleRate, 1024, 512);
     phaseFX.prepare(sampleRate, samplesPerBlock);
     postFX.prepare(sampleRate, samplesPerBlock);
     //...Debugging
     DBG("prepareToPlay: entered");
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

     // ...Debugging
     DBG("processBlock: start");
     DBG("processBlock size: " << buffer.getNumSamples());
     DBG("playbackPosition: " << playbackPosition);

     // Get parameters
     float pitch = *parameters.getRawParameterValue("pitchShift");
     float stretch = *parameters.getRawParameterValue("timeStretch");
     bool bypassSpectral = parameters.getRawParameterValue("bypassSpectral")->load();
     bool bypassPhase = parameters.getRawParameterValue("bypassPhase")->load();

     // Apply parameters
     vocoder.setPitchShiftRatio(pitch);
     for (const auto metadata : midiMessages)
     {
          const auto msg = metadata.getMessage();
          if (msg.isNoteOn())
          {
               handleMidiPitch(msg.getNoteNumber());
          }
     }
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
          // ...Debugging
          DBG("Before fftProcessor.pushSample()");
          fftProcessor.pushSample(sample);
          DBG("After fftProcessor.pushSample()");

          if (fftProcessor.readyToProcess())
          {
               DBG("Ready to process FFT");
               fftProcessor.performSTFT();
               DBG("Performed STFT"); //...Debugging

               // --- STN Analysis (Mask-based Routing Logic) ---
               juce::AudioBuffer<float> magnitude;

               // ...Debugging
               DBG("STN analysis...");
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

               // Update STN gain settings
               float sinGain = *parameters.getRawParameterValue("sinGain");
               float transGain = *parameters.getRawParameterValue("transGain");
               float resGain = *parameters.getRawParameterValue("resGain");
               stnModule.setSinusoidGain(sinGain);
               stnModule.setTransientGain(transGain);
               stnModule.setResidualGain(resGain);

               // --- STN Analysis (Mask-based Routing Logic) ---
               DBG("Analyzing STN"); //...Debugging
               stnModule.analyze(magnitude);
               auto &sMask = stnModule.getSinusoidMask();
               auto &tMask = stnModule.getTransientMask();
               auto &rMask = stnModule.getResidualMask();

               // (Optional) apply masks to fftProcessor.real/imag
               stnModule.recombineMaskedBuffers(fftProcessor.real, fftProcessor.imag);

               // --- Vocoder ---
               DBG("Before vocoder"); //...Debugging
               vocoder.process(fftProcessor.real, fftProcessor.imag);
               DBG("After vocoder");
               // --- FX Chain ---
               spectralFX.processBins(fftProcessor.real, fftProcessor.imag);
               phaseFX.process(fftProcessor.real, fftProcessor.imag);
               fftProcessor.performISTFT();
               DBG("Performed ISTFT"); //...Debugging
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