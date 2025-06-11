// FFTProcessor.cpp
#include "FFTProcessor.h"

FFTProcessor::FFTProcessor(int size, int hop)
    : fftSize(size), hopSize(hop), fft(static_cast<int>(std::log2(size)))
{
     prepare(fftSize, hopSize);
}

void FFTProcessor::prepare(int size, int hop)
{
     fftSize = size;
     hopSize = hop;
     fft = juce::dsp::FFT(static_cast<int>(std::log2(size)));
     window = std::make_unique<juce::dsp::WindowingFunction<float>>(
         fftSize, juce::dsp::WindowingFunction<float>::hamming);

     inputBuffer.assign(fftSize, 0.0f);
     outputBuffer.assign(fftSize, 0.0f);
     real.setSize(1, fftSize);
     imag.setSize(1, fftSize);

     inputIndex = 0; // ensure indices are reset
     outputIndex = 0;

     jassert(real.getNumChannels() == 1 && real.getNumSamples() == fftSize);
     jassert(imag.getNumChannels() == 1 && imag.getNumSamples() == fftSize);
     jassert(window != nullptr);

     DBG("FFTProcessor::prepare - real: " << real.getNumChannels() << " x "
                                          << real.getNumSamples());
}

void FFTProcessor::pushSample(float sample)
{
     if (inputIndex >= fftSize)
          inputIndex = 0; // bounds safety
     inputBuffer[inputIndex++] = sample;
}

bool FFTProcessor::readyToProcess() const
{
     return inputIndex == 0;
}

void FFTProcessor::performSTFT()
{
     DBG("FFTProcessor::performSTFT");

     jassert(real.getNumChannels() == 1 && real.getNumSamples() == fftSize);
     jassert(imag.getNumChannels() == 1 && imag.getNumSamples() == fftSize);
     jassert(window != nullptr);

     juce::FloatVectorOperations::copy(real.getWritePointer(0), inputBuffer.data(),
                                       fftSize);
     juce::FloatVectorOperations::clear(imag.getWritePointer(0), fftSize);
     window->multiplyWithWindowingTable(real.getWritePointer(0), fftSize);

     fft.performRealOnlyForwardTransform(real.getWritePointer(0));

     for (int i = 0; i < fftSize / 2; ++i)
     {
          float r = real.getSample(0, i * 2);
          float im = real.getSample(0, i * 2 + 1);
          real.setSample(0, i, r);
          imag.setSample(0, i, im);
     }

     DBG("FFTProcessor::performSTFT complete");
}

void FFTProcessor::performISTFT()
{
     DBG("FFTProcessor::performISTFT");

     jassert(real.getNumChannels() == 1 && real.getNumSamples() == fftSize);
     jassert(imag.getNumChannels() == 1 && imag.getNumSamples() == fftSize);
     jassert(window != nullptr);

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

     DBG("FFTProcessor::performISTFT complete");
}

float FFTProcessor::popSample()
{
     if (outputIndex >= fftSize)
          return 0.0f;
     return outputBuffer[outputIndex++];
}
