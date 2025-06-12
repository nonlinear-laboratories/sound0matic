// STNModule.cpp

#include "STNModule.h"
#include "Morphology.h"

STNModule::STNModule()
{
}

void STNModule::analyze(const juce::AudioBuffer<float> &magnitude)
{
     Morphology::extractComponents(magnitude, sinusoidMask, transientMask,
                                   residualMask);
}

void STNModule::recombineMaskedBuffers(juce::AudioBuffer<float> &real,
                                       juce::AudioBuffer<float> &imag)
{
     for (int ch = 0; ch < real.getNumChannels(); ++ch)
     {
          float *realData = real.getWritePointer(ch);
          float *imagData = imag.getWritePointer(ch);
          auto *sM = sinusoidMask.getReadPointer(ch);
          auto *tM = transientMask.getReadPointer(ch);
          auto *rM = residualMask.getReadPointer(ch);
          for (int i = 0; i < real.getNumSamples(); ++i)
          {
               float wS = sinusoidGain * sM[i];
               float wT = transientGain * tM[i];
               float wR = residualGain * rM[i];
               float total = wS + wT + wR + 1e-6f; // avoid div0
               realData[i] =
                   (wS * realData[i] + wT * realData[i] + wR * realData[i]) / total;
               imagData[i] =
                   (wS * imagData[i] + wT * imagData[i] + wR * imagData[i]) / total;
          }
     }
}
