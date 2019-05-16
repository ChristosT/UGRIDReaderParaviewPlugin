# UGRID mesh reader plugin for Paraview


Visualize meshes in the [ugrid format](http://www.simcenter.msstate.edu/software/downloads/doc/ug_io/) in [Paraview](https://www.paraview.org).

Tested using Paraview 5.6.0 on Linux.

## Instructions
1. The installation can be done using the [paraview superbuild system](https://github.com/lhofmann/paraview-superbuild-docker).
Using the provided Dockerfile build the superbuild system or download it directly from [dockerhub](https://hub.docker.com/r/lhofmann/paraview-superbuild).
2. Start the image using the provided `run.sh` script.
2. Start the container and navigate to `/mnt/shared` which is mounted under the current directory.
3. Run `mkdir build  && cd build && cmake ../src/ && make ` to build the plugin. This should produce a `.so` library which you can load from paraview throught the
`Tools->Manage Plug-ins` menu.
4. The `File->Open` menu should now show `ugrid` files in the available formats.


