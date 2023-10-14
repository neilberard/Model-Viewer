# Model-Viewer
![model_viewer_progress](https://user-images.githubusercontent.com/6820590/105642251-7f7f5780-5e3d-11eb-9372-8d2461f9f71a.jpg)

Simple OpenGL model viewer. Supports importing FBX models.  

This is pet project based on the following tutorials online:

-https://learnopengl.com/

-https://www.youtube.com/channel/UCQ-W1KE9EYfdxhL6S4twUNw

Requires: CMake version 3.8 or greater and MSVC v142 installed with Visual Studio. 

Instructions:
Run in the command line.
git clone --recurse-submodules "https://github.com/neilberard/Model-Viewer.git"

For Visual Studio Solution and compile .exe files: run Build.bat
For Visual Studio solution only: run GenerateProject.bat

To view in shaded mode IBL map must be loaded.

Only .hdr files supported for IBL maps (no alpha channel)

HDR Images can be found at:
http://www.hdrlabs.com/sibl/archive.html
www.textures.com
