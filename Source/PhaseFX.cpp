// PhaseFX.cpp
#include "PhaseFX.h"

PhaseFX::PhaseFX()
{
}
PhaseFX::~PhaseFX()
{
}

void PhaseFX::prepare(double sampleRate, int samplesPerBlock)
{
     fs = sampleRate;
     juce::ignoreUnused(samplesPerBlock);
}

void PhaseFX::process(juce::AudioBuffer<float> &real, juce::AudioBuffer<float> &imag)
{
     if (bypass)
          return;

     for (int channel = 0; channel < real.getNumChannels(); ++channel)
     {
          auto *r = real.getWritePointer(channel);
          auto *i = imag.getWritePointer(channel);
          int n = real.getNumSamples();

          for (int j = 0; j < n; ++j)
          {
               float mag = std::sqrt(r[j] * r[j] + i[j] * i[j]);
               float phase = std::atan2(i[j], r[j]);

               // Example phase manipulation: freeze phase
               // phase = 0.0f;

               r[j] = mag * std::cos(phase);
               i[j] = mag * std::sin(phase);
          }
     }
}

void PhaseFX::setBypass(bool shouldBypass)
{
     bypass = shouldBypass;
}

bool PhaseFX::isBypassed() const
{
     return bypass;
}
