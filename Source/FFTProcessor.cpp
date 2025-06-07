// FFTProcessor.cpp
#include "FFTProcessor.h"

FFTProcessor::FFTProcessor(int size, int hop)
    : fft((int)std::log2(size)), fftSize(size), hopSize(hop)
{
     inputBuffer.resize(fftSize, 0.0f);
     outputBuffer.resize(fftSize, 0.0f);
     real.setSize(1, fftSize);
     imag.setSize(1, fftSize);

     window = std::make_unique<juce::dsp::WindowingFunction<float>>(
         fftSize, juce::dsp::WindowingFunction<float>::hamming);
}

void FFTProcessor::prepare(int size, int hop)
{
     fftSize = size;
     hopSize = hop;
     inputIndex = 0;
     outputIndex = 0;

     inputBuffer.assign(fftSize, 0.0f);
     outputBuffer.assign(fftSize, 0.0f);
     real.setSize(1, fftSize);
     imag.setSize(1, fftSize);

     window = std::make_unique<juce::dsp::WindowingFunction<float>>(
         fftSize, juce::dsp::WindowingFunction<float>::hamming);
}

void FFTProcessor::pushSample(float sample)
{
     if (inputIndex < fftSize)
          inputBuffer[inputIndex++] = sample;
}

bool FFTProcessor::readyToProcess() const
{
     return inputIndex >= fftSize;
}

void FFTProcessor::performSTFT()
{
     juce::FloatVectorOperations::copy(real.getWritePointer(0), inputBuffer.data(),
                                       fftSize);
     juce::FloatVectorOperations::clear(imag.getWritePointer(0), fftSize);
     window->multiplyWithWindowingTable(real.getWritePointer(0), fftSize);
     fft.performRealOnlyForwardTransform(real.getWritePointer(0));
     // real + imag are packed interleaved
     for (int i = 0; i < fftSize / 2; ++i)
     {
          float r = real.getSample(0, i * 2);
          float im = real.getSample(0, i * 2 + 1);
          real.setSample(0, i, r);
          imag.setSample(0, i, im);
     }
}

void FFTProcessor::performISTFT()
{
     for (int i = 0; i < fftSize / 2; ++i)
     {
          real.setSample(0, i * 2, real.getSample(0, i));
          real.setSample(0, i * 2 + 1, imag.getSample(0, i));
     }

     fft.performRealOnlyInverseTransform(real.getWritePointer(0));
     window->multiplyWithWindowingTable(real.getWritePointer(0), fftSize);

     for (int i = 0; i < fftSize; ++i)
          outputBuffer[i] = real.getSample(0, i);

     outputIndex = 0;
     inputIndex = 0;
}

float FFTProcessor::popSample()
{
     if (outputIndex < outputBuffer.size())
          return outputBuffer[outputIndex++];
     return 0.0f;
}
