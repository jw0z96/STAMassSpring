# STA Mass Spring TODO & Priorities

## Priorities

1. prioritise GPU RK4 (compute shader approach)
- 1. one shader thread per spring
- 2. store mass positions in a texture (size = number of masses) Vec3 float
<!-- - 3. store mass velocities in a texture (size = number of masses) Vec3 float -->
- 3. store spring 'state' (position & velocity) in a texture (size = number of springs) Vec3 float
- 4. store spring start and end indices in a texture (size = number of springs) vec2 uint
- 5. store spring resting length (float) in a texture (size = number of springs) single float
- 6. all springs need a spring constant (float) through a uniform
- 7. all springs need a damping constant (float) through a uniform
- 8. all springs need the timestep (float) through a uniform

2. different integration schemes
3. external forces and different deformation

## TODO

- collisions

## LINKS

- https://github.com/lemms/SiggraphAsiaDemo2012 Pixar presented GPU Mass-spring