<script
  src="https://cdn.mathjax.org/mathjax/latest/MathJax.js?config=TeX-AMS-MML_HTMLorMML"
  type="text/javascript">
</script>

# Exaggerated Shading Interactive
Interactive OpenGL program implementing [Xshade](https://pixl.cs.princeton.edu/pubs/Rusinkiewicz_2006_ESF/exaggerated_shading.pdf) 
 (Szymon Rusinkiewicz, Michael Burns, and Doug DeCarlo.
*Exaggerated Shading for Depicting Shape and Detail.*
ACM Transactions on Graphics (Proc. SIGGRAPH) 25(3), July 2006.).

![Demo](./images/demo.gif)

# Method

*Exaggerated Shading for Depicting Shape and Detail* proposes an original shading technique for revealing details at all frequencies, independent of light position and surface orientation. 

This shading method is introduced as a form of "Multiscale local toon shading", and is based off of techniques used in **cartographic terrain relief**.

## Key Ideas

1. Lighting is clamped as a cel shader to emphasize large bends, oppposeed to gradually lighted rendering. In practice we use a "smooth clamp".

2. Lighting is computed at multiple scales using surface normals smoothed by different amounts to convey overall shape and details at different frequencies.

3. Light direction varies as if it is always at a **grazing** position with respect to the overall orientation of that part of the object, bringing out detail of the object.

## Implementation

The shading method proposed in the paper requires several levels of smoothed normals to be preprocessed, each using a progressively larger Gaussian filter for smoothing of the normals.

For each scale, the standard deviation of the Gaussian filter is increased exponentially by a constant scalar coefficient. The paper suggests $\sqrt(2)$ for each level.

$ \sigma_{n+1} = \sqrt(2)\sigma_n $




### Principal Direction-based Light Adjustment

The basic rendering model simply projects the light source at each point into the tangent plane perpendicular to the smoothed normal. Instead with a formulization of "contrast" proposed in the paper as 



## Dependencies

Written in C++ using OpenGL and GLFW3.

Used [ImGui](https://github.com/ocornut/imgui) for interface purposes.

Used [Assimp](https://github.com/assimp/assimp) for loading 3D models.

Built using [vcpkg](https://github.com/microsoft/vcpkg) on Windows.

