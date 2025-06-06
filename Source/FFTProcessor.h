// FFTProcessor.h

#pragma once
#include <JuceHeader.h>

class FFTProcessor
{
   public:
     FFTProcessor();
     void prepare(int fftSize, int hopSize);
     void processBlock(juce::AudioBuffer<float> &input,
                       juce::AudioBuffer<float> &output);

   private:
     std::unique_ptr<juce::dsp::FFT> fft;
     juce::AudioBuffer<float> fftInputBuffer;
     juce::AudioBuffer<float> fftOutputBuffer;
     juce::AudioBuffer<float> overlapAddBuffer;
     std::vector<float> window;

     int fftSize = 0;
     int hopSize = 0;
     int fftOrder = 0;
     int writePosition = 0;
};
