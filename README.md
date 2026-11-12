# 3D Software Renderer

A CPU-based software renderer for animated 3D objects.

## Requirements

- C++17-compatible compiler
- SDL2 development libraries
- JsonCpp development libraries
- GNU Make

## Building

Build the project:

```sh
make
```

Rebuild or clean:

```sh
make rebuild
make clean
```

### Optional build parameters

| Parameter | Description |
|---|---|
| `DEBUG=1` | Build in debug mode (GDB). |
| `DBG_ASAN=1` | Enable address sanitizing. |
| `DBG_PRINTS=1` | Enable debug prints. |
| `DBG_POLY=1` | Display polygon meshes and their normals for debugging. |

Pass an option to `make`, for example `make DEBUG=1`.

## Running the engine

```sh
./bin/3D_engine --file ./config/config.json --conf config
```
or:
```sh
cd ./bin/
./3D_engine --file ../config/config.json --conf config
```
usage:
```sh
./3D_engine --help
```

version:
```sh
./3D_engine --version
```

## Concepts

### Coordinate system

| Axis | Positive | Negative | Zero |
|---|---|---|---|
| X | Right | Left | Screen center |
| Y | Up | Down | Screen center |
| Z | Toward the viewer (outside) | Away from the viewer (inside) | Screen plane |

The camera sits outside the screen plane and looks inward, like the viewer.
The light source also sits outside the screen plane, higher up, illuminating inward.

- Polygons facing the camera are visible; polygons facing away are culled.
- **Focal length** controls perspective projection and magnification.
- Objects closer to the camera appear larger; objects farther from the camera appear smaller.
- Objects behind the camera are not displayed.
- **Epsilon** determines near-plane/depth tolerance.

### Scene hierarchy

Consider an animated airplane scene:

- A movable **trim tab** belongs to and moves with an aileron or elevator.
- An **aileron** and a **flap** belong to and move with a wing.
- A stationary **wing** belongs to and moves with the airplane.
- An **elevator** belongs to and moves with a horizontal stabilizer.
- A stationary **horizontal stabilizer** belongs to and moves with the airplane.
- The movable **airplane** belongs to and moves with a gimbal.
- The movable **gimbal** belongs to the **World**.
- The runway and background may also belong to the World.
- The **World** is the root of the hierarchy.

A child inherits its parent's transformations while retaining its own local transformations.

#### Example hierarchy tree

```text
World
  ├──Gimbal
  │    └──Airplane
  │         ├──Right Wing
  │         │    ├──Aileron
  │         │    │      └──Trim
  │         │    └──Flap
  │         ├──Left Wing
  │         │    ├──Aileron
  │         │    │    └──Trim
  │         │    └──Flap
  │         ├──Right Horizontal Stabilizer
  │         │    └──Elevator
  │         │         └──Trim
  │         ├──Left Horizontal Stabilizer
  │         │    └──Elevator
  │         │         └──Trim
  │         ├──Vertical Stabilizer
  │         │    └──Rudder
  │         │         └──Trim
  │         ├──Propeller
  │         │    ├──Blade 1
  │         │    ├──Blade 2
  │         │    ├──Blade 3
  │         │    └──Blade 4
  │         ├──Right Landing Gear
  │         │    └──Wheel
  │         ├──Left Landing Gear
  │         │    └──Wheel
  │         └──Front Landing Gear
  │              └──Wheel
  └──Land
       ├──Background
       │    ├──Control Tower
       │    ├──Buildings
       │    └──Mountains
       └──Landing Strip
```

## JSON configuration

Scenes and their hierarchical relationships are defined in JSON configuration files located in the config/ directory.

The scene hierarchy is described in a JSON configuration file. Usually under ../config/ .

See [JSON configuration](./JSON.md) for details.
