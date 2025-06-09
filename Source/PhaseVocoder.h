// PhaseVocoder.h
#pragma once
#include <JuceHeader.h>
#include <complex>
#include <vector>

class PhaseVocoder
{
   public:
     PhaseVocoder();
     ~PhaseVocoder();

     void prepare(double sampleRate, int fftSize, int hopSize);
     void process(juce::AudioBuffer<float> &real, juce::AudioBuffer<float> &imag);

     void setTimeStretchRatio(float ratio);
     void setPitchShiftRatio(float ratio);

   private:
     double sampleRate = 44100.0;
     int fftSize = 1024;
     int hopSize = 512;

     float timeStretchRatio = 1.0f;
     float pitchShiftRatio = 1.0f;

     std::vector<std::vector<float>> previousPhase;
     std::vector<std::vector<float>> phaseAccumulator;

     void ensureInternalBuffers(int numChannels);
     float principalArg(float phase);
};
