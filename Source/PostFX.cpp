// PostFX.cpp - ADSR, drive, limiter
#include "PostFX.h"

PostFX::PostFX()
{
}

void PostFX::prepare(double, int)
{
}

void PostFX::applyGain(juce::AudioBuffer<float> &buffer)
{
     buffer.applyGain(outputGain);
}

void PostFX::setOutputGain(float newGain)
{
     outputGain = newGain;
}

float PostFX::getOutputGain() const
{
     return outputGain;
}
