SET scriptsPath=%~dp0
%VULKAN_SDK%\Bin\glslc.exe -fshader-stage=vertex %scriptsPath%\..\src\VertexShader.glsl -o %scriptsPath%\..\compiled\VertexShader.spv
%VULKAN_SDK%\Bin\glslc.exe -fshader-stage=fragment %scriptsPath%\..\src\FragmentShader.glsl -o %scriptsPath%\..\compiled\FragmentShader.spv