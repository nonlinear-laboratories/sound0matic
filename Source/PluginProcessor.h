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
                             public juce::AudioProcessorValueTreeState::Listener
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
     void parameterChanged(const juce::String &id, float newValue) override
     {
          if (id == "sinGain")
               stnModule.setSinusoidGain(newValue);
          else if (id == "transGain")
               stnModule.setTransientGain(newValue);
          else if (id == "resGain")
               stnModule.setResidualGain(newValue);
     }

   private:
     SampleLoader sampleLoader;
     int playbackPosition = 0;
     juce::AudioBuffer<float> workingBuffer;
     FFTProcessor fftProcessor{1024, 512};
     STNModule stnModule;
     PhaseVocoder vocoder;
     int currentMidiNote = 60;      // default to Middle C (C4)
     int currentMidiVelocity = 127; // default to maximum velocity
     int currentMidiChannel = 1;    // default to channel 1
     void handleMidiPitch(int midiNote);
     SpectralFX spectralFX;
     PhaseFX phaseFX;
     PostFX postFX;

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sound0maticProcessor)
};
