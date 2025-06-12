// FFTProcessor.h

#pragma once
#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>

class FFTProcessor
{
   public:
     FFTProcessor(int fftSize, int hopSize);
     void prepare(int fftSize, int hopSize);
     void pushSample(float sample);
     bool readyToProcess() const;
     void performSTFT();
     void performISTFT();
     float popSample();

     juce::AudioBuffer<float> real, imag;

   private:
     int fftSize = 0;
     int hopSize = 0;
     int inputIndex = 0;
     int outputIndex = 0;

     std::vector<float> inputBuffer, outputBuffer;

     juce::dsp::FFT fft;
     std::unique_ptr<juce::dsp::WindowingFunction<float>> window;
};
