// STNModule.h
#pragma once
#include <JuceHeader.h>

class STNModule
{
   public:
     STNModule();

     void analyze(const juce::AudioBuffer<float> &magnitude);

     const juce::AudioBuffer<float> &getSinusoidMask() const;
     const juce::AudioBuffer<float> &getTransientMask() const;
     const juce::AudioBuffer<float> &getResidualMask() const;

   private:
     juce::AudioBuffer<float> sinusoidMask;
     juce::AudioBuffer<float> transientMask;
     juce::AudioBuffer<float> residualMask;
};
