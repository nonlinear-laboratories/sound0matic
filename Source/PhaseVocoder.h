// PhaseVocoder.h
#pragma once
#include <JuceHeader.h>

class PhaseVocoder
{
   public:
     PhaseVocoder();

     void prepare(double sampleRate, int fftSize, int hopSize);
     void process(const juce::AudioBuffer<float> &input,
                  juce::AudioBuffer<float> &output);

     void setTimeStretchRatio(float ratio); // <- missing
     void setPitchShiftRatio(float ratio);  // <- missing

   private:
     double sampleRate = 44100.0;
     int fftSize = 1024;
     int hopSize = 512;
     float timeStretchRatio = 1.0f;
     float pitchShiftRatio = 1.0f;

     // Add internal buffers, FFT data, and phase accumulator here
};
