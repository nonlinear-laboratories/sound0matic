// STNModule.cpp
#include "STNModule.h"
#include "Morphology.h"

STNModule::STNModule()
{
}

void STNModule::analyze(const juce::AudioBuffer<float> &magnitude)
{
     const int width = magnitude.getNumSamples();
     const int height = magnitude.getNumChannels();

     CImg<float> spectrogram(width, height, 1, 1);
     for (int y = 0; y < height; ++y)
          for (int x = 0; x < width; ++x)
               spectrogram(x, y) = magnitude.getSample(y, x);

     CImg<float> sinImg, transImg, residualImg;

     Morphology::extractComponents(spectrogram, sinImg, transImg, residualImg);
     convertToBuffer(sinImg, sinusoidMask, sinusoidGain);
     convertToBuffer(transImg, transientMask, transientGain);
     convertToBuffer(residualImg, residualMask, residualGain);

     Morphology::extractComponents(magnitude, sinusoidMask, transientMask,
                                   residualMask);
     recombineNeeded = true;
}

void STNModule::setSinusoidGain(float gain)
{
     if (std::abs(gain - sinusoidGain) > 1e-3f)
          recombineNeeded = true;
     sinusoidGain = gain;
}
void STNModule::setTransientGain(float gain)
{
     if (std::abs(gain - transientGain) > 1e-3f)
          recombineNeeded = true;
     transientGain = gain;
}
void STNModule::setResidualGain(float gain)
{
     if (std::abs(gain - residualGain) > 1e-3f)
          recombineNeeded = true;
     residualGain = gain;
}

const juce::AudioBuffer<float> &STNModule::getSinusoidMask() const
{
     return sinusoidMask;
}

const juce::AudioBuffer<float> &STNModule::getTransientMask() const
{
     return transientMask;
}

const juce::AudioBuffer<float> &STNModule::getResidualMask() const
{
     return residualMask;
}

void STNModule::recombineMaskedBuffers(juce::AudioBuffer<float> &real,
                                       juce::AudioBuffer<float> &imag)
{
     if (!recombineNeeded && sinusoidGain == lastSinGain &&
         transientGain == lastTransGain && residualGain == lastResGain)
          return;

     lastSinGain = sinusoidGain;
     lastTransGain = transientGain;
     lastResGain = residualGain;
     recombineNeeded = false;

     for (int ch = 0; ch < real.getNumChannels(); ++ch)
     {
          float *realData = real.getWritePointer(ch);
          float *imagData = imag.getWritePointer(ch);
          const float *sMask = sinusoidMask.getReadPointer(ch);
          const float *tMask = transientMask.getReadPointer(ch);
          const float *rMask = residualMask.getReadPointer(ch);

          for (int i = 0; i < real.getNumSamples(); ++i)
          {
               float s = sMask[i] * sinusoidGain;
               float t = tMask[i] * transientGain;
               float r = rMask[i] * residualGain;
               float sum = s + t + r + 1e-6f; // avoid divide-by-zero

               realData[i] =
                   (s * realData[i] + t * realData[i] + r * realData[i]) / sum;
               imagData[i] =
                   (s * imagData[i] + t * imagData[i] + r * imagData[i]) / sum;
          }
     }
}
