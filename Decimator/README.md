This is the source for a bitcrusher plugin. This was meant to be a jumping off point for learning JUCE, but there's quite a lot I'd like to do with it.

Parameters:

  Decimation: Downsampling factor. A decimation of 10 will reduce the signal to 1/10 the number of samples.
  
  Bit Depth: Amount of quantisation applied to the amplitude of the signal. There will be 2^(Bit Depth) levels that the sample can be quantised to, so a lower bit depth reduces the resolution of the sample.
  
  Jitter: A random factor applied to the sample. Not too happy with this currently.
  
  Mix: Wet-Dry mix, or the blend of bitcrushed (wet) to clean (dry) samples outputted.
  

  TODO:
  
    Currently, the DSP acts sample-by-sample. It would be good to come back and pull everything out into a function (or method) that acts on channels, even if there's still a for loop over samples at the core.
    
    Vectorisation - not for pre-optimisation, but just to see how it works (such as with SIMDregister).
    
    Harmonic Noise or "Harshness". I think this would be a good feature to add more noise, but needs a better understanding of JUCE's waveshapers to implement.
    
    "Smoothing". How much each sample should be smoothed with subsequent samples using a moving average.

    Jitter - I think I want to change this to apply random sizing to the downsampling. Or this could apply a factor to more than just 1 sample at a time, as it's currently quite harsh.
    
    GUI. I've had some good results with the initial UI, and I'm happier with it compared to a default, but there's a lot more to be done to create a good 'feel'. This needs some defining of what I'd like it to look like. 
          Some things I'd like to implement/look into: Level meters, joint dials and sliders, resizable graphics, shading
          Some good plugins I enjoy the GUI of: Ableton default, Goodhertz, Codec

          
