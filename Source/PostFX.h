// PostFX.h
#pragma once
#include <JuceHeader.h>

class PostFX
{
   public:
     PostFX();
     void prepare(double sampleRate, int samplesPerBlock);
     void applyGain(juce::AudioBuffer<float> &buffer);

     void setOutputGain(float newGain);
     float getOutputGain() const;

   private:
     float outputGain = 1.0f;
};
