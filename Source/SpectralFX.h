// SpectralFX.h
#pragma once
#include <JuceHeader.h>

class SpectralFX
{
   public:
     SpectralFX();
     void processBins(juce::AudioBuffer<float> &real, juce::AudioBuffer<float> &imag);

     void setBypass(bool shouldBypass);
     bool isBypassed() const;

   private:
     bool bypass = false;
};
