# Maps

"Maps" are an environment full of entities. Right now there are two supported types of entity: 
level geometry (`lvgeo`) and static props (`sp`).

Each line of a map's `.map` file defines one entity. The first word of the line is the type of 
entity.

## Level Geometry (lvgeo)
Level geometry is the floor, walls, etc that make up the structure of the map. Maps should have 
one level geometry entity. The geometry can't be moved, rotated or scaled. They're treated 
specially by the engine so ink colouring can work later.

A level geometry entity looks like this:
```
lvgeo level0
```

`lvgeo` is the type of entity (level geometry). The second word - `level0` in this case - is 
the name of the model file containing the actual geometry itself. This should be a file existing 
in the project's `models` folder. So, if we had `models/level0.obj`, that would be converted to 
`models/level0.3mdl` at build time, which is then loaded as the model for the level geometry 
entity in this map.

## Static Prop (sp)
Static props are simple entities that can have a 3D model, a position, rotation, and scale. They 
do not have any further behaviours at this time. You can (and should) have loads of them.

A static prop looks like this:
```
sp ramp p  3.0 0.0  6.0 r 0.0 180.0 0.0 s 2.0 2.0 2.0
```

`sp` says that this is a static prop. `ramp` is the model file to use for this prop - make sure 
it exists in `models/`. You can and should re-use models as much as you want, since the engine 
will only load one copy of each and re-use it.

`p 0.x 0.y 0.z` is the position of the prop. This controls where in the map the 3D model will 
appear. Units are arbitrary.

`r 0.x 0.y 0.z` is the rotation of the prop, about each of the three axes. Units are degrees.

`s 0.x 0.y 0.z` scales the prop up or down along each of its axes, with 1.0 being no scaling. 
In this example, the size of the ramp is doubled to make a 2x2x2 unit ramp from a 1x1x1 unit model.
