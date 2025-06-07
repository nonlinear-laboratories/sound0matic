// PluginProcessor.h

#pragma once

#include <JuceHeader.h>

#include "FFTProcessor.h"
#include "PhaseFX.h"
#include "PhaseVocoder.h"
#include "PostFX.h"
#include "STNModule.h"
#include "SampleLoader.h"
#include "SpectralFX.h"

class Sound0maticProcessor : public juce::AudioProcessor
{
   public:
     Sound0maticProcessor();
     ~Sound0maticProcessor() override;

     void prepareToPlay(double sampleRate, int samplesPerBlock) override;
     void releaseResources() override;

     bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
     void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

     juce::AudioProcessorEditor *createEditor() override;
     bool hasEditor() const override;

     const juce::String getName() const override;

     bool acceptsMidi() const override;
     bool producesMidi() const override;
     bool isMidiEffect() const override;
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
     FFTProcessor fftProcessor{1024, 512}; // or whatever value
     STNModule stnModule;
     PhaseVocoder vocoder;
     SpectralFX spectralFX;
     PhaseFX phaseFX;
     PostFX postFX;

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sound0maticProcessor)
};
