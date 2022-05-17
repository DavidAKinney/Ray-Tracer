As the primary course project for CSCI 5607 (Intro to Computer Graphics), this ray tracer program has gone through
four major iterations that introduced the following features:

- Iteration 1: Basic sphere and ellipsoid rendering
- Iteration 2: Light sources and shadows
- Iteration 3: Rendering triangles and texturing
- Iteration 4: Reflections and transparency

The ray tracer is built entirely in C++ without any prewritten code outside of the standard C library. It takes an input 
file describing a scene, renders it, and writes the resulting image to an ASCII .ppm file. In order to compile, link, and 
run the ray tracer, please follow the steps below:

1. With a command line terminal, change into the ray tracer's "Source Code" directory.
2. Run the "make" command. This will compile and link the ray tracer if the required C++ libraries are present.
3. Call "./ray_tracer filepath" where "filepath" is the path from the "Source Code" directory to the desired scene file.
4. Once the scene is rendered, the resulting .ppm image file will be written to the same location as the original 
   scene file. These .ppm files can be opened with an image editor such as GIMP.
5. In order to efficiently remove the executable files from memory, a "make clean" command can be called from the 
   "Source Code" directory.

Note: Please read "Scene File Template" in the "Notes" directory for a scene file syntax guide.

This ray tracer does not make use of bounding volume hierarchy or any other efficiency techniques. This causes more complex
images with multiple reflective surfaces and shadows to take several minutes to render. In the "Casting.h" header file, 
there are six macro definitions that can be enabled to increase the quality of shadows and ray recursions. These values can
increase the runtime even further however, so they are currently disabled in the source code. Due to this simplicity, the
best way to increase runtime is to define scenes files with lower resolutions.
