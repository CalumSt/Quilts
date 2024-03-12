/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace ParameterID{
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);
    // it would be good to come back and see what this is exactly doing
    PARAMETER_ID(bitDepth)
    PARAMETER_ID(numberOfDecimationSamples)
    PARAMETER_ID(jitterMix)
    PARAMETER_ID(wetMix)
    
    #undef PARAMETER_ID
}

//==============================================================================
/**
*/
class BitcrusherAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    BitcrusherAudioProcessor();
    ~BitcrusherAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //==============================================================================

    // Using the tree to manage parameters - see JUCE book. This seems slightly more robust for future features

    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:

    juce::AudioParameterFloat* bitDepthParam;
    juce::AudioParameterFloat* decimationParam;
    juce::AudioParameterFloat* jitterMixParam;
    juce::AudioParameterFloat* wetMixParam;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::Random random;

    auto BitcrusherAudioProcessor::applyQuantisationBySample(float numberOfLevels, float sample);
    auto BitcrusherAudioProcessor::applyMixBySample(float mixFraction, float wetSample, float drySample);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitcrusherAudioProcessor)
};
