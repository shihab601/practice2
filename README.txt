MOVIE THEATRE - CSE COMPUTER GRAPHICS PROJECT
================================================

Based on the supplied Lighting sample project structure and the supplied
Illumination Model and Shading lecture slide.

Features:
1. 3D modelling transformations: translate, rotate, scale.
2. Viewing transformation: Camera / view matrix + perspective projection.
3. Moving object: ceiling fan rotates continuously; F toggles fan.
4. Two kinds of light:
   - Two Point Lights
   - One Spot Light mounted near the projector
5. Multiple object/material colors.
6. Phong Surface Rendering:
   Ambient + Diffuse + Specular terms are calculated per fragment.

Files:
main.cpp
shader.h
camera.h
basic_camera.h
pointLight.h
sphere.h
vertexShaderForPhongShading.vs
fragmentShaderForPhongShading.fs
vertexShader.vs
fragmentShader.fs

CONTROLS
W/A/S/D = move camera
Arrow keys = look around
Mouse + RIGHT button = look around
Mouse wheel = zoom
F = fan ON/OFF
L = point lights ON/OFF
P = projector spotlight ON/OFF
ESC = exit

VISUAL SCENE
- Movie screen and stage
- 32 theatre chairs
- Projector and lens
- Rotating 5-blade ceiling fan
- Ceiling/floor/walls
- Two colored point lights
- Projector spotlight
