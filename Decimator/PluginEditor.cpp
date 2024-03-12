/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BitcrusherAudioProcessorEditor::BitcrusherAudioProcessorEditor (BitcrusherAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    juce::LookAndFeel::setDefaultLookAndFeel(&globalLNF);

    bitDepthKnob.label = "Bit Depth";
    bitDepthKnob.unit = "Bits";
    addAndMakeVisible(bitDepthKnob);

    decimationKnob.label = "Decimation";
    decimationKnob.unit = "Factor";
    addAndMakeVisible(decimationKnob);

    jitterMixKnob.label = "Jitter";
    jitterMixKnob.unit = "%";
    addAndMakeVisible(jitterMixKnob);

    wetMixKnob.label = "Mix";
    wetMixKnob.unit = "%";
    addAndMakeVisible(wetMixKnob);


    setSize (260, 300);
}

BitcrusherAudioProcessorEditor::~BitcrusherAudioProcessorEditor()
{
}

//==============================================================================
void BitcrusherAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);

    juce::Font mainFont("Corbel", 15.0f, juce::Font::plain);
    g.setFont (mainFont);
}

void BitcrusherAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // 
    // I think this can be done a lot better without as much hardcoding

    constexpr int padding = 20;

    juce::Rectangle r(padding, padding, 100, 120);
    bitDepthKnob.setBounds(r);

    juce::Rectangle rDecimation = r.withX(r.getRight() + padding);
    decimationKnob.setBounds(rDecimation);

    juce::Rectangle rNoiseMix = r.withY(r.getBottom() + padding);
    jitterMixKnob.setBounds(rNoiseMix);

    juce::Rectangle rWetMix = rDecimation.withY(rDecimation.getBottom() + padding);
    wetMixKnob.setBounds(rWetMix);
    //wetMixKnob.setCentrePosition({ rWetMix.getX(), rWetMix.getCentreY() });
}
