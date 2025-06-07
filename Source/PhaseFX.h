// PhaseFX.h
#pragma once
#include <JuceHeader.h>

class PhaseFX
{
   public:
     PhaseFX();
     ~PhaseFX();

     void prepare(double sampleRate, int samplesPerBlock);
     void process(juce::AudioBuffer<float> &real, juce::AudioBuffer<float> &imag);

     void setBypass(bool shouldBypass);
     bool isBypassed() const;

   private:
     bool bypass = false;
     double fs = 44100.0;
};
