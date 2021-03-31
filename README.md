# My Inverse Kinematics

## Description

This is a simple inverse kinematics demonstration written in C++ and OpenGL. It implements a linear arm made up of 6 bones, and it will try to touch the target represented by a spinning cube. Poses are updated at each frame based on Jacobian transpose method. The root joint is restricted to be a one dimension joint to show how limited DOF influence the system.

## Build

The project is managed using Visual Studio on Win10. It depends on OpenGL, GLEW and GLM. With these dependencies configured correctly, this project should also be able to run on OS X and Linux. Instructions can be found [here](http://ivl.calit2.net/wiki/index.php/BasecodeCSE167F20).

## Usage

- Press `W`, `A`, `S` and `D` to move the target around.
- Press left `Shift` and left `Ctrl` to move the target up and down.
- Press `Space` to pause the movement of the arm.
- Press `P` to turn on and off polygon view.

## Artworks!

Here are some GIFs of the running program. Try download it and play with the mechanical arm!

![](https://cdn.jsdelivr.net/gh/TonyZYT2000/ImageHost@master/IK1.gif)

![](https://cdn.jsdelivr.net/gh/TonyZYT2000/ImageHost@master/IK2.gif)