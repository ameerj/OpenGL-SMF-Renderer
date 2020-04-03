# OpenGL SMF Renderer
This is an introduction to OpenGL for me. Building off of the foundation provided through the 
[OpenGL series by YouTuber "TheCherno"](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)

I followed the series up to the video on integrating ImGui, which was still in 2D.

# My Goal
To build on this foundation to be able to import several 3D .SMF models. My intention is to be able to transform these models to create
a scene, and use the scene transformation parameters as arguments in a raytracer I will be building to recreate the same scene there.

An example of a cube in the .SMF format:
```
v -0.50000 -0.50000 -0.500000
v 0.50000 -0.50000 -0.500000
v 0.50000 0.50000 -0.500000
v -0.50000 0.50000 -0.500000
v -0.50000 -0.50000 0.500000
v 0.50000 -0.50000 0.500000
v 0.50000 0.50000 0.500000
v -0.50000 0.50000 0.500000
f 1 3 2
f 1 4 3
f 8 6 7
f 8 5 6
f 1 8 4
f 1 5 8
f 2 7 6
f 2 3 7
f 1 6 5
f 1 2 6
f 4 7 3
f 4 8 7
```

# To run Locally
Open the solution in VS 2019. 

Press F5.



If there are linking errors, try the following:

Open the Property Manager by going to `View -> Other Windows -> Property Manager`

Right click on the `OpenGL` project, and click `Add Existing Property Sheet`

Locate the `PropertySheet.props` file found in the OpenGL project directory, and click open.

You should now be able to run my project.
