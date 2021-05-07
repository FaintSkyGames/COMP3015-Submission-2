# 2021-COMP3015-CW1 Initial Prototype 

### Development Details
* Coded in Visual Studio 2019 Version 16.8
* Built on Windows 10 Education Edition

### How to build in Visual Studio
* Open Project_Template.sln
* Clean solution
* Build solution
* Build is found in the x64 folder and then Debug folder

### How to run from build
* Locate build folder location
* Select Project_Template.exe

### How does it work?
scenebasic_uniform.cpp and scenebasic_uniform.h are the important scripts as they determine how to set up and render the code. However, this project heavily relies on shaders to create 3 key techniques - shadows, geometry outlines and noise. 

The shadows is split up into 3 programs and use a combination of vertex, fragment and geometry shaders. The first program calculates the volumes. 

The second program calculates the rendering but comes in two variations, one for geometry outlines(named renderProg) and another for noise(named flatProg). Both are set up in pass 2 and then switched between in drawScene so that the geometry shader is applied to the objects and the noise is active on the planes.

The third program is a composite shader.

Dynamism has been added with a moving light. This updates the light position if the program is animating. Dynamism is also added through making the UFO rotate and move up and down.

### Current bugs & code that should be changed
* Error appears in console when running. Doesn’t affect output currently.

### Link to summary video
https://youtu.be/nuOrvnTD2J4

### Assets
* Fence: "Low Poly wood fence on grass" (https://skfb.ly/6WMBp) by cattleya is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/).
* Pig: "Spot" (keenan@cs.caltech.edu) by Keenan Crane
* Grass: "Grass" (https://www.brusheezy.com/textures/20185-seamless-green-grass-textures) 
* Dirt: "Dirt" (https://seamless-pixels.blogspot.com/2012/09/free-seamless-ground-textures.html)
* UFO: "Ufo" (https://skfb.ly/LUZH) by yanix is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/).


