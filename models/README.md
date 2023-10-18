# 3mdl models
We convert all the .obj files (a normal 3D model format) into custom .3mdl files using the obj_convert tool in the tools/ folder.

Any .obj files placed into this folder will be automatically converted and included in the romfs.

## Rationale

3D models are cool, and so is the .obj file format, but it has a key problem on 3DS - it can mix and match indexes for the positions, normals, and uv. The 3DS wants all attributes to have the same index. Apparently this is an issue for OpenGL too, which is very funny.

Thus we need to preprocess the models to get all the vertexes split up and re-optimised. Since we're doing that, might as well also process the text-based .obj format into a binary format that's closer to what the 3DS GPU wants to see.

Thus: 3mdl. It's not really a real format yet, just whatever is closest to the internal representation the Splatoon 3DS engine is using at this stage in development. It has changed often and it will change again. I have no idea how animations will work.

## Usage
In Blender, make your cool thing. Don't bother setting material properties, the engine doesn't support them (yet?). Do bother smoothing the normals. File->Export->Wavefront (.obj), select only UV Coordinates and Normals in the Geometry panel. You'll get a .mtl and .obj file like the ones in this folder.

Run `make`. It'll create tools/obj_convert for you, and also convert all the models in this folder. If you want to manually convert a model, it's `./obj_convert gaming.obj gaming.3mdl`.

Results go on the root of the SD card for now, that'll move soon.

NOTE: level geometry should be named `levelX.obj`. It won't get indexed so that the ink colouring can work properly.
