# DLREngine
This project was developed by me during Dragons Lake rendering internship. Althought it's called an engine, but in actuality it's more of a program that showcases some interesting rendering techniques. D3D11 is required to run it. Right now there is one hardcoded scene, that starts displaying after starting the project.
# Features
* Physically Based Deferred Renderer
* Shadow Mapping for Point and Spot Lights
* IBL precompute and caching
* Optimal Instanced Rendering of opaque meshes
* HDR with ACES Tonemapping
* CPU Particles for smoke
* GPU Particles on mesh destruction
* Decals
* Wind waves for grass
* Tinted smoke particles
# Control keys
* <kbd>WASD</kbd> to fly around
* <kbd>Shift</kbd> to speed up
* Hold <kbd>LMB</kbd> to rotate camera
* <kbd>N</kbd> key to spawn instance of horse model in cursor location
* <kbd>G</kbd> key to spawn decals on mesh pointed by cursor
* Hold <kbd>RMB</kbd> over model to drag it
* <kbd>M</kbd> key to remove instance of mesh while spawning GPU particles
# Improvements that should be made 
* Separate engine code and application code in different projects
* Add minimal editor with models and textures manipulation, different scenes, etc
* General interface for rendering, that would allow to add more rendering APIs
* Add dependecies as git submodules
# Screenshots
![image](https://github.com/tsnazar/DLREngine/assets/70116589/12ab0a1c-cb41-41db-a652-5e41b1f5d55e)
![image](https://github.com/tsnazar/DLREngine/assets/70116589/501202f1-f0b1-45e5-b03d-bd7390456913)
![image](https://github.com/tsnazar/DLREngine/assets/70116589/9b6e29da-1dbf-435e-97da-a14fbc75b8bb)
![image](https://github.com/tsnazar/DLREngine/assets/70116589/67a3e6ba-c6a8-43b3-bb85-a8a5ad025ac5)
![image](https://github.com/tsnazar/DLREngine/assets/70116589/167f828d-33cf-4146-931d-d9a8a31ffc1f)

