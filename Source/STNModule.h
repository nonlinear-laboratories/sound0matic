// STNModule.h
#pragma once
#include <JuceHeader.h>

class STNModule
{
   public:
     STNModule();
     void analyze(const juce::AudioBuffer<float> &magnitude);
     void recombineMaskedBuffers(juce::AudioBuffer<float> &real,
                                 juce::AudioBuffer<float> &imag);

     const juce::AudioBuffer<float> &getSinusoidMask() const noexcept
     {
          return sinusoidMask;
     }
     const juce::AudioBuffer<float> &getTransientMask() const noexcept
     {
          return transientMask;
     }
     const juce::AudioBuffer<float> &getResidualMask() const noexcept
     {
          return residualMask;
     }

     void setSinusoidGain(float gain) noexcept { sinusoidGain = gain; }
     void setTransientGain(float gain) noexcept { transientGain = gain; }
     void setResidualGain(float gain) noexcept { residualGain = gain; }

   private:
     juce::AudioBuffer<float> sinusoidMask, transientMask, residualMask;
     float sinusoidGain = 1.0f, transientGain = 1.0f, residualGain = 1.0f;
};
