// PluginEditor.cpp
#include "PluginEditor.h"
#include "PluginProcessor.h"

Sound0maticEditor::Sound0maticEditor(Sound0maticProcessor &p)
    : AudioProcessorEditor(p), processor(p) // Use initializer list
{
     setSize(400, 300);

     titleLabel.setText("Sound0matic", juce::dontSendNotification);
     titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
     titleLabel.setJustificationType(juce::Justification::centred);
     addAndMakeVisible(titleLabel);
}

Sound0maticEditor::~Sound0maticEditor()
{
}

void Sound0maticEditor::paint(juce::Graphics &g)
{
     g.fillAll(juce::Colours::black);
     g.setColour(juce::Colours::white);
     g.setFont(15.0f);
     g.drawFittedText("sound0matic", getLocalBounds(), juce::Justification::centred, 1);
}

void Sound0maticEditor::resized()
{
     titleLabel.setBounds(10, 10, getWidth() - 20, 40);
}
