# Path-Tracer
##What this project is:

This project is a fairly simple reverse ray-tracing renderer, written in C++. It generates a series of random spheres, and uses the principle of reverse ray tracing (or path tracing) to render an image of those spheres.

![Sample Image](https://i.imgur.com/VWLTA9w.png)

Currently, the simulation only supports spheres as a shape, but it handles three main material types (Lambertian, Metal, and Dielectric) and is flexible enough to be able to generate random new spheres every time it is run.

##Notes on the code.
The first thing you'll probably notice about this code is that all but the main file are header files, with only the main file being a .cpp. This is because the objects used in this project are templated.
The second thing you'll probably notice is that most of the objects used in this project are templated. If I were being a purist about making a path tracer I'd likely have stuck to one type, but I also wanted practice with templated objects to make sure I can use them properly. The datatype used is set under the aliases near the top of the main file.

The configuration of the simulation itself is handled by a series of "configuration constants" defined at the very top of the main in PathTracer.cpp. These can be altered to make high or low quality images, change the position and orientation of the camera, change the number of spheres generated, etc.
As the program runs, and after these values are read, the program instantiates the five set spheres (four larger ones visible in the sample image with a fifth acting as the ground), and then generates a series of random spheres on the ground around.
It then simulates reverse ray tracing, pixel by pixel, to determine the colour seen at a particular point in the output image.

##Notes on Ray Tracing
This is just a brief summary on the method of reverse raytracing in case anyone is not familiar.
In the real world, when we see an object, it is because light from a source collided with that object, was randomly scattered, and just happened to fly in the direction of our eyes. It hits our eyes and our brain translates that into an image.
This is the process we are simulating to draw our image. However, if we simulate that process as it is, we would be wasting considerable resources simulating all the light rays which collide with an object, and don't go anywhere near our eyes. While this is happening all the time in reality (and the vast, vast majority of the time no less), if we want efficiency we should only select rays to simulate which will contribute to the final image.
So instead of simulating rays from the light source, to the object (or objects), and then to our eyes; we choose to do it backwards. We simulate the rays going out of our eyes and colliding with the objects. That way, we guarantee that every single ray we simulate will contribute to the final image.
This reversing the process and simulating it is what reverse ray-tracing is.
