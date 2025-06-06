// SpectralFX.cpp
#include "SpectralFX.h"

SpectralFX::SpectralFX()
{
}

void SpectralFX::processBins(juce::AudioBuffer<float> &real,
                             juce::AudioBuffer<float> &imag)
{
     if (bypass)
          return;

     for (int channel = 0; channel < real.getNumChannels(); ++channel)
     {
          float *realData = real.getWritePointer(channel);
          float *imagData = imag.getWritePointer(channel);
          int numSamples = real.getNumSamples();

          for (int i = 0; i < numSamples; ++i)
          {
               float magnitude =
                   std::sqrt(realData[i] * realData[i] + imagData[i] * imagData[i]);
               float phase = std::atan2(imagData[i], realData[i]);

               // Example: scale magnitude
               magnitude *= 0.5f;

               // Recombine
               realData[i] = magnitude * std::cos(phase);
               imagData[i] = magnitude * std::sin(phase);
          }
     }
}

void SpectralFX::setBypass(bool shouldBypass)
{
     bypass = shouldBypass;
}

bool SpectralFX::isBypassed() const
{
     return bypass;
}
