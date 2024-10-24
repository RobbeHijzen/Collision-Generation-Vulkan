# Vulkan Engine with automatic Collision Generation

## Overview
This project started off as an assignment for one of my courses at **Digital Arts and Entertainment**, but has since been transformed into my own personal project.

With that said, this project is split into two different parts, one is the **Engine**, and the other is the **Collision Generation**. Let's dive into both of these.


## Vulkan Engine
### Overview
The engine part of the project is designed to primarily render meshes into the scene. It takes care of all the communication between the CPU and the GPU.
### Features
- Easy-to-use rendering system
- Instancing on objects
- Component System
- Event System

## Collision Generation
### Overview
The goal of the Collision Generation was to automatically generate collisions on any object you give it. 

### Features
- Automatic Collision generation
- Collision-Based physics which blocks objects from going through each other.
- Player character with movement and gravity, with which you can walk around the scene.


### How it Works
The way it works is that you assign a CollisionComponent to a mesh, which then, like an octree, creates Axis Aligned Bounding Boxes (AABBs for short) that describe the collision of the mesh.
These AABBs are then used in a "CollisionFixer" to dissalow objects with collision to pass through each other by solving any collisions that happened that frame.

## Dependencies
- C++ 20 or later
- <a href="https://vulkan.lunarg.com/sdk/home" target="_blank">Vulkan SDK</a>
- CMake (for building)
