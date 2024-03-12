/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"
#include<JuceHeader.h>

//==============================================================================
BitcrusherAudioProcessor::BitcrusherAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    castParameter(apvts, ParameterID::bitDepth, bitDepthParam);
    castParameter(apvts, ParameterID::numberOfDecimationSamples, decimationParam);
    castParameter(apvts, ParameterID::jitterMix, jitterMixParam);
    castParameter(apvts, ParameterID::wetMix, wetMixParam);
    // May want to add a listener if some parameters use more complex skew such as pow or exp - see JUCE book 170
}

BitcrusherAudioProcessor::~BitcrusherAudioProcessor()
{
}

//==============================================================================
const juce::String BitcrusherAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BitcrusherAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BitcrusherAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BitcrusherAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BitcrusherAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BitcrusherAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BitcrusherAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BitcrusherAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BitcrusherAudioProcessor::getProgramName (int index)
{
    return {};
}

void BitcrusherAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BitcrusherAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Layout of parameters - Add a noise generator and "harshness" i.e. harmonic distortion

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::bitDepth, "Bits",
        juce::NormalisableRange<float>(1.0f, 16.0f, 0.1f, 0.5f),
        16.0f,
        juce::AudioParameterFloatAttributes().withLabel("Bits")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::numberOfDecimationSamples, "Decimation",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("Samples")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::jitterMix, "Noise",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(ParameterID::wetMix, "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));
    // TODO : Look at juce book for how %s are implemented

    return layout;
}

//==============================================================================
void BitcrusherAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int downsampleIndex = 0;
}

void BitcrusherAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BitcrusherAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BitcrusherAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    //=============================================================================
    // bit depth parameters
    float bitDepth = bitDepthParam->get();
    float totalDiscreteLevels = powf(2, bitDepth);
    //=============================================================================
    // downsampling parameters - previous version used sample rate - decimation bypasses the need to get sample rate
    auto downsampleStep = decimationParam->get();
   
    // do NOT want to hear what's going to happen if this drops below 0
    if (downsampleStep < 1) {
        downsampleStep = 1;
    }

    // intialise this here so it doesn't complain later
    float firstSample = 0.0f;

    //=============================================================================
    // Noise Mix Parameters

    float jitterMix = jitterMixParam->get() / 100;

    //=============================================================================
    // wet-dry mix parameters
    float wetMix = wetMixParam->get() / 100;

    //=============================================================================

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Want to add some safety checks before and after

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        int downsampleIndex = 0;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float currentValue = channelData[sample];

            // downsampling rough implementation

            if (downsampleIndex >= downsampleStep) {
                downsampleIndex = 0;
            }

            if (downsampleIndex == 0) {
                auto firstSample = currentValue;
                ++downsampleIndex;
            }

            else if (downsampleIndex > 0) {
                currentValue = firstSample;
                ++downsampleIndex;
            }

            // quantisation
            currentValue = applyQuantisationBySample(totalDiscreteLevels, currentValue);

            // jitter
            float randomValue = currentValue * random.nextFloat();
            currentValue = applyMixBySample(jitterMix, randomValue, currentValue);


            // mix
            channelData[sample] = applyMixBySample(wetMix, currentValue, channelData[sample]);
            ;

            // An alternative approach: multiply by 10, round, divide by 10 > changes resolution of sample
            // channelData[sample] = (std::round(10 * channelData[sample])) / 10;

            // TODO: Smoothing: basically a local area average for audio data pre-mix?
            // TODO: Harmonic Distortion generator
            // TODO: White Noise
            
            // TODO: Vectorisation - convert functions below from sample by sample to block by block
            
            // want to add some ear protection in case of clipping, infs or nans

            channelData[sample] = protectYourEarsSample(channelData[sample]);
        }

    }
}


auto BitcrusherAudioProcessor::applyQuantisationBySample(float numberOfLevels, float sample)
{
    float remainder = fmodf(sample, 1 / numberOfLevels);
    return sample - remainder;
}

auto BitcrusherAudioProcessor::applyMixBySample(float mixFraction, float wetSample, float drySample)
{
    return (wetSample * mixFraction) + ((1 - mixFraction) * drySample)
}

//==============================================================================
bool BitcrusherAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BitcrusherAudioProcessor::createEditor()
{
    return new BitcrusherAudioProcessorEditor(*this);
}

//==============================================================================
void BitcrusherAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BitcrusherAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitcrusherAudioProcessor();
}
