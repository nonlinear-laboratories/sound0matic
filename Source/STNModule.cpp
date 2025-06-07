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
