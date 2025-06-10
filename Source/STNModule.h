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

     void setSinusoidGain(float gain);
     void setTransientGain(float gain);
     void setResidualGain(float gain);

     bool recombineNeeded = true;
     float lastSinGain = -1.0f;
     float lastTransGain = -1.0f;
     float lastResGain = -1.0f;

     void recombineMaskedBuffers(juce::AudioBuffer<float> &real,
                                 juce::AudioBuffer<float> &imag);

   private:
     juce::AudioBuffer<float> sinusoidMask;
     juce::AudioBuffer<float> transientMask;
     juce::AudioBuffer<float> residualMask;
     float sinusoidGain = 1.0f;
     float transientGain = 1.0f;
     float residualGain = 1.0f;

     void convertToBuffer(const cimg_library::CImg<float> &img,
                          juce::AudioBuffer<float> &buffer, float gain);
};
