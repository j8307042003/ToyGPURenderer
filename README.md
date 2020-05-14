

# My WIP toy path tracing renderer accelerated via gpu.

using vulkan compute shader to compute each pixel's color( ray tracing, shading, sample integrator).


## Requirement
cmake

vulkan sdk(Molten on mac)

visual studio(windows)


## Build

### Mac
modify this line and put your Molten library path in CMakeLists.txt

```bash	
	#define Molten sdk path here!
	Set(MacVulkanSDK /Users/pine/lib/vulkansdk-macos-1.1.130.0)
```

### Mac and windows
go to build folder
```bash
cmake ..
cmake --build .
```

