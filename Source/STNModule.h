// STNModule.h
#pragma once
#include <JuceHeader.h>

class STNModule
{
   public:
     STNProcessor() = default;
     ~STNProcessor() = default;

     void processBlock(juce::AudioBuffer<float> &buffer)
     {
          // TODO: Implement STN processing
     }
};
