/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "CustomSlider.h"
#include "LookAndFeel.h"


//==============================================================================
/**
*/
class BitcrusherAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BitcrusherAudioProcessorEditor (BitcrusherAudioProcessor&);
    ~BitcrusherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BitcrusherAudioProcessor& audioProcessor;

    LookAndFeel globalLNF;

    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;

    RotaryKnob bitDepthKnob;
    SliderAttachment bitDepthAttachment{ audioProcessor.apvts, ParameterID::bitDepth.getParamID(), bitDepthKnob.slider };

    RotaryKnob decimationKnob;
    SliderAttachment decimationAttachment{ audioProcessor.apvts, ParameterID::numberOfDecimationSamples.getParamID(), decimationKnob.slider };

    RotaryKnob jitterMixKnob;
    SliderAttachment jitterMixAttachment{ audioProcessor.apvts, ParameterID::jitterMix.getParamID(), jitterMixKnob.slider };

    RotaryKnob wetMixKnob;
    SliderAttachment wetMixAttachment{ audioProcessor.apvts, ParameterID::wetMix.getParamID(), wetMixKnob.slider };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitcrusherAudioProcessorEditor)
};
