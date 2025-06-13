// PluginProcessor.h
#pragma once

#include "FFTProcessor.h"
#include "PhaseFX.h"
#include "PhaseVocoder.h"
#include "PostFX.h"
#include "STNModule.h"
#include "SampleLoader.h"
#include "SpectralFX.h"
#include <JuceHeader.h>

#define JucePlugin_Name "sound0matic"

class Sound0maticProcessor : public juce::AudioProcessor,
                             private juce::AudioProcessorValueTreeState::Listener
{
   public:
     Sound0maticProcessor();
     ~Sound0maticProcessor() override;

     juce::AudioProcessorValueTreeState parameters;

     static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
     void prepareToPlay(double sampleRate, int samplesPerBlock) override;
     void releaseResources() override;
     bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
     void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

     juce::AudioProcessorEditor *createEditor() override;
     bool hasEditor() const override;

     const juce::String getName() const override;
     bool acceptsMidi() const override { return true; }
     bool producesMidi() const override { return false; }
     bool isMidiEffect() const override { return false; }
     double getTailLengthSeconds() const override;

     int getNumPrograms() override;
     int getCurrentProgram() override;
     void setCurrentProgram(int index) override;
     const juce::String getProgramName(int index) override;
     void changeProgramName(int index, const juce::String &newName) override;

     void getStateInformation(juce::MemoryBlock &destData) override;
     void setStateInformation(const void *data, int sizeInBytes) override;

   private:
     SampleLoader sampleLoader;
     int playbackPosition = 0;
     juce::AudioBuffer<float> workingBuffer;
     juce::Synthesiser synth;
     std::unique_ptr<juce::SamplerSound> samplerSound;
     void parameterChanged(const juce::String &parameterID, float newValue) override;
     void setupSynth(double sampleRate);

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sound0maticProcessor)
};
