// PhaseVocoder.cpp
#include "PhaseVocoder.h"
#include <cmath>

PhaseVocoder::PhaseVocoder()
{
}
PhaseVocoder::~PhaseVocoder()
{
}

void PhaseVocoder::prepare(double newSampleRate, int newFftSize, int newHopSize)
{
     sampleRate = newSampleRate;
     fftSize = newFftSize;
     hopSize = newHopSize;

     previousPhase.clear();
     phaseAccumulator.clear();
}

void PhaseVocoder::setTimeStretchRatio(float ratio)
{
     timeStretchRatio = ratio;
}

void PhaseVocoder::setPitchShiftRatio(float ratio)
{
     pitchShiftRatio = ratio;
}

float PhaseVocoder::principalArg(float phase)
{
     while (phase > juce::MathConstants<float>::pi)
          phase -= 2.0f * juce::MathConstants<float>::pi;
     while (phase < -juce::MathConstants<float>::pi)
          phase += 2.0f * juce::MathConstants<float>::pi;
     return phase;
}

void PhaseVocoder::ensureInternalBuffers(int numChannels)
{
     if (previousPhase.size() != numChannels)
     {
          previousPhase.resize(numChannels);
          phaseAccumulator.resize(numChannels);

          for (int ch = 0; ch < numChannels; ++ch)
          {
               previousPhase[ch].resize(fftSize / 2 + 1, 0.0f);
               phaseAccumulator[ch].resize(fftSize / 2 + 1, 0.0f);
          }
     }
}

void PhaseVocoder::process(juce::AudioBuffer<float> &real,
                           juce::AudioBuffer<float> &imag)
{
     int numChannels = real.getNumChannels();
     int numBins = real.getNumSamples();

     ensureInternalBuffers(numChannels);

     for (int ch = 0; ch < numChannels; ++ch)
     {
          auto *realData = real.getWritePointer(ch);
          auto *imagData = imag.getWritePointer(ch);

          for (int bin = 0; bin < numBins; ++bin)
          {
               float re = realData[bin];
               float im = imagData[bin];

               float magnitude = std::sqrt(re * re + im * im);
               float phase = std::atan2(im, re);

               float deltaPhase = phase - previousPhase[ch][bin];
               previousPhase[ch][bin] = phase;

               float expectedDelta =
                   2.0f * juce::MathConstants<float>::pi * bin * hopSize / fftSize;
               float unwrapped =
                   principalArg(deltaPhase - expectedDelta) + expectedDelta;

               phaseAccumulator[ch][bin] += unwrapped * timeStretchRatio;

               float outPhase = phaseAccumulator[ch][bin];
               realData[bin] = magnitude * std::cos(outPhase);
               imagData[bin] = magnitude * std::sin(outPhase);
          }
     }
}
