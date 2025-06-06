// FFTProcessor.cpp

#include "FFTProcessor.h"

FFTProcessor::FFTProcessor()
{
}

void FFTProcessor::prepare(int fftSize_, int hopSize_)
{
     fftSize = fftSize_;
     hopSize = hopSize_;
     fftOrder = static_cast<int>(std::log2(fftSize));
     fft = std::make_unique<juce::dsp::FFT>(fftOrder);

     fftInputBuffer.setSize(1, fftSize * 2); // Circular buffer
     fftOutputBuffer.setSize(1, fftSize);    // Time-domain output
     overlapAddBuffer.setSize(1, fftSize);   // Accumulator
     fftInputBuffer.clear();
     fftOutputBuffer.clear();
     overlapAddBuffer.clear();

     window.resize(fftSize);
     juce::dsp::WindowingFunction<float>::fillWindowingTables(
         window.data(), fftSize, juce::dsp::WindowingFunction<float>::hamming);

     writePosition = 0;
}

void FFTProcessor::processBlock(juce::AudioBuffer<float> &input,
                                juce::AudioBuffer<float> &output)
{
     const float *in = input.getReadPointer(0);
     float *out = output.getWritePointer(0);
     int numSamples = input.getNumSamples();

     for (int i = 0; i < numSamples; ++i)
     {
          fftInputBuffer.setSample(0, writePosition, in[i]);
          writePosition++;

          if (writePosition >= fftSize)
          {
               // Copy windowed frame
               juce::AudioBuffer<float> frame(1, fftSize);
               for (int j = 0; j < fftSize; ++j)
               {
                    float sample =
                        fftInputBuffer.getSample(0, writePosition - fftSize + j);
                    frame.setSample(0, j, sample * window[j]);
               }

               // Prepare FFT buffers
               std::vector<float> fftData(2 * fftSize, 0.0f); // interleaved real/imag
               std::memcpy(fftData.data(), frame.getReadPointer(0),
                           sizeof(float) * fftSize);

               fft->performRealOnlyForwardTransform(fftData.data());

               // Placeholder: no processing, just passthrough
               fft->performRealOnlyInverseTransform(fftData.data());

               // Apply window again & accumulate
               for (int j = 0; j < fftSize; ++j)
               {
                    float val = fftData[j] * window[j] / fftSize; // normalize
                    overlapAddBuffer.setSample(0, j,
                                               overlapAddBuffer.getSample(0, j) + val);
               }

               // Output hopSize samples
               for (int j = 0; j < hopSize; ++j)
               {
                    out[i - hopSize + j + 1] = overlapAddBuffer.getSample(0, j);
               }

               // Shift overlap buffer
               for (int j = 0; j < fftSize - hopSize; ++j)
               {
                    overlapAddBuffer.setSample(
                        0, j, overlapAddBuffer.getSample(0, j + hopSize));
               }
               overlapAddBuffer.clear(0, fftSize - hopSize, hopSize);

               writePosition = 0; // reset for next frame
          }
     }
}
