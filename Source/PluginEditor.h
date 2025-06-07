// PluginEditor.h
#pragma once
#include "PluginProcessor.h"
#include <JuceHeader.h>

class Sound0maticEditor : public juce::AudioProcessorEditor
{
   public:
     Sound0maticEditor(Sound0maticProcessor &p);
     ~Sound0maticEditor() override;

     void paint(juce::Graphics &) override;
     void resized() override;

   private:
     Sound0maticProcessor &processor; // Not processorRef
     juce::Label titleLabel;          // Add this field since you're using it

     JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sound0maticEditor)
};
