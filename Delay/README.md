This is the source for a delay plugin based on this Audio Programmer tutorial: https://www.youtube.com/watch?v=2oCb3SXBcTI

Important: This build is unsafe. It currently crashes on close/deletion on the plugin, I think due to some memory allocation issues or dangling pointers. The original used raw pointers, which caused issues if you deleted the plugin mid audio process. This build doesn't do that, but still experiences runtime errors.

There are no parameters yet, since I want to get the core DSP working (safely) first. I'm going to look at alternative implementations to see if there's a better way or I can fix this.

31/03/24:
The above tutorial is actually based on this implementation: https://github.com/ffAudio/ffTapeDelay/tree/master
          
