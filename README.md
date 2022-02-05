# opengl-hello-world
A mini project in openGL that simulates a planet system with lights rotations and textures.


Code of this project is adapted from Joey De Vries tutorial ![learnopengl](https://learnopengl.com/)


## libraries and compilation 

The libraries needed for the execution are already compiled. These are Assimp and GLFW.
The libraries for the mathematical calculations and also for the version of openGL GLM, GLAD are in header files and they are compiled together with the executable.

The following must be installed before compiling:

`sudo apt-get build-essential install libsoil-dev libglm-dev libassimp-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev libxi-dev mesa-common-dev libglm-dev`

For the compilation only the `make` command is enough


## Executable

The executable is the `./mainexe`




## Controls 

`P` -> pause

`C` -> continue

`ESC` -> exit program

In addition, an adjustment has been made for the speed of rotation of the boxes and the planet.
The increase and decrease of speed is done with the Q and E buttons.

`Q` -> increase rotational speed

`E` -> reduction of rotation speed


### Control of the camera

`W` -> move front on the z axis

`S` -> move back on the z axis

`A` -> move right on the x-axis

`D` -> move left on the x-axis

`UP ARROW` -> move up on the y axis

`DOWN ARROW` -> move down on the y axis

`RIGHT ARROW` -> rotate right on the y axis

`LEFT ARROW` -> rotate left on the y axis

## About the implementation

A planet object has been used for the main model.
This model performs a circular motion around (0,0,0).
Inside this object is placed the light source that follows the exact orbit of the object.

Boxes prbot around the planet.
Their center of their rotation is the center of the planet but the axes of rotation are different beyond the y-axis on which the planet moves. 
# Graphics
# Graphics
