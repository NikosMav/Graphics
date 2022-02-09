# Κινούμενος φωτισμός Phong
A mini project in openGL that simulates a face lamp system using Phong's lighting model.


Code of this project is adapted from tutorial ![learnopengl](https://learnopengl.com/)


## libraries and compilation 

The libraries needed for the execution are already compiled. These are Assimp and GLFW.
The libraries for the mathematical calculations and also for the version of openGL GLM, GLAD are in header files and they are compiled together with the executable.

The following command must be used before compiling to secure that we have the proper libraries:

`sudo apt-get install build-essential libsoil-dev libglm-dev libassimp-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev libxi-dev mesa-common-dev libglm-dev`

For the compilation only the `make` command is enough


## Executable

The executable is the `./main`


## Controls 

`ESC` -> exit program

`H` -> increase rotational speed

`J` -> reduction of rotation speed


### Control of the camera

`W` -> move front on the z axis

`S` -> move back on the z axis

`A` -> move right on the x-axis

`D` -> move left on the x-axis

In addition to WSAD keys the camera is also controlled using the mouse. 


## About the implementation

Using learnopengl's basic implementations and modifing them to purpose our needs, so that we have a working enviroment for use. In addition to that, lab tests and documentations helped a lot to understand the assignment.

Auxiliary functions were used to implement certain behaviors.

Inside the render loop:
A woman object has been used for the main model.
This model is situated at (0,0,0) and light is being reflected on it using proper shader models and implementations of the Phong's algorithm. 
A light source is being placed near the main model which is performing an orbit around it. Light from the sphere reflects on the main model while moving around it.
The sphere was built using the given files (Sphere.cpp & Sphere.h) as a mesh of triangles and lines.
