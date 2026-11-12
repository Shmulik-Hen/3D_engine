# JSON Configuration

**Parsing** verifies JSON syntax, field types, field names, and fixed-length arrays.\
**Validation** checks duplicates, completeness, value constraints, references, and scene hierarchy.

### Name/Value constraints

<a id="valid1"></a>(1) **valid field names:** Non-null, non-empty, not-control chars, lower-case ASCII only `letters` + `underscore`\
<a id="valid2"></a>(2) **valid string values:** Non-null, non-empty, not-control chars, upper/lower-case\
&emsp;&emsp;&emsp;ASCII/Unicode `letters`, `numbers` and `punctuations`, except `[()[]{}',.;:"\]`\
<a id="valid3"></a>(3) **valid numeric values:** See [`inc/constants.h`](./inc/constants.h).

## Sections

### Version

`version` is **mandatory** and contains exactly three unsigned integers.&ensp;[(3)](#valid3)\
The currently supported version is **`[0, 0, 3]`**.

### Library

The library contains definitions of **polygons**, **meshes**, and **elements**, with at least one of each type in a valid configuration.

- Each element references one valid 3D mesh.
- Each mesh references at least one valid polygon.
- Each polygon has **3–8 consecutive vectors** describing a **planar, closed shape**.
- Vertex winding order (**CW/CCW**) determines which side of a polygon is visible.
- Multiple elements can reuse the same mesh.
- Object and field order is not significant.
- Field names must be lowercase.
- Duplicate object names are not allowed.

#### `library.polygons`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Valid object identifier.&ensp;[(2)](#valid2)|
| `force` | Boolean | Render the polygon regardless of its facing direction. |
| `color_idx` | Unsigned integer | Color palette index (see below).&ensp;[(3)](#valid3)|
| `vectors` | Array | At least three vectors defining a valid polygon.|

**Vector:** is an unnamed array containing **exactly three floating-point coordinates**: `[x, y, z]`.\
**Coordinate valid values:** [ -1000.0 - 1000.0 ].&ensp;[(3)](#valid3)

#### `library.meshes`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Valid object identifier.&ensp;[(2)](#valid2)|
| `polygons` | Array of strings | Valid names of at least one valid polygon.&ensp;[(2)](#valid2)|

#### `library.elements`

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Valid object identifier.&ensp;[(2)](#valid2)|
| `mesh` | String | Valid name of exactly one valid mesh.&ensp;[(2)](#valid2)|

#### Color palette

| Index | Color | Index | Color |
|---:|---|---:|---|
| 0 | Black | 8 | Lime |
| 1 | Grey | 9 | Green |
| 2 | Silver | 10 | Aqua |
| 3 | White | 11 | Teal |
| 4 | Red | 12 | Blue |
| 5 | Maroon | 13 | Navy |
| 6 | Yellow | 14 | Magenta |
| 7 | Olive | 15 | Purple |

### Scenes

`scenes` is an array containing **at least one scene** in a valid configuration.

- Each scene contains a list of elements and optional environment and runtime settings.
- The elements form a hierarchy tree with **exactly one root element** and **at least one other child element**.
- Multiple children may share a parent (siblings).
- The root is identified by a `null` parent.
- Child elements may reference a library element, and multiple scene elements may reuse the same library definition.
- Scene and element ordering is not significant.
- Field names must be lowercase.
- Duplicate object names are not allowed.

#### Scene elements

| Field | JSON type | Description |
|---|---|---|
| `name` | String | Valid scene-element identifier.&ensp;[(2)](#valid2)|
| `parent` | String or null | Valid name of a valid parent; `null` identifies the root. [(2)](#valid2)|
| `active` | Boolean | Enables or disables the scene element. |
| `ref` | String | Optional; Valid name of a valid library element.&ensp;[(2)](#valid2)|
| `origin` | Array | Optional; Initial position and orientation attributes. (see below) |
| `motion` | Array | Optional; Continuous translation/rotation attributes. (see below) |

##### Attributes

Each attribute is an array containing **exactly seven floating-point values**:
Attribute fields:

```text
[ X Rotation, Y Rotation, Z Rotation, X Translation, Y Translation, Z Translation, Scale factor ]
```

The scale factor is `1.0` for normal size, `0.5` for half size, and `2.0` for twice the size.

**Rotation/Translation valid values:** `[ -1000.0 - 1000.0 ]`.&ensp;[(3)](#valid3)\
**Scale values:** `[ 0.1 - 10.0 ]`.
###### Default: `[ 0, 0, 0, 0, 0, 0, 1 ]`.&ensp;[(3)](#valid3)

#### Environment

`environment` is an **optional JSON object** setting the graphical properties.\
All its fields are optional overrides.

| Field | JSON type | Description |
|---|---|---|
| `resolution` | Array | Contains exactly two unsigned integers defining the window size.|
| `color_idx` | Unsigned integer | Background color index (see above).&ensp;[(3)](#valid3) |
| `light_type` | Unsigned integer | `0`: directional (sunlight); `1`: point light; (default: `0`).&ensp;[(3)](#valid3)|
| `light_direction` | Vector | Direction of the light source (default: `[ 0, 0, 1 ]`).&ensp;[(3)](#valid3)|
| `light_position` | Vector | Position of the light source (default: `[ 0, 20, 75 ]`).&ensp;[(3)](#valid3)|
| `camera_position` | Vector | Position of the camera (default: `[ 0, 0, 1000 ]`).&ensp;[(3)](#valid3)|
| `focal_len` | Floating-point number | Perspective projection and magnification (default: `2000`).&ensp;[(3)](#valid3)|
| `near_eps` | Floating-point number | Near-plane/depth tolerance (default: `1`).&ensp;[(3)](#valid3)|

**Minimum supported resolution:** `800 × 600` (4:3).&ensp;[(3)](#valid3)\
**Maximum supported resolution:** `1920 × 1080` (16:9).&ensp;[(3)](#valid3)\
**Default resolution:** `1280 × 800` (16:10).&ensp;[(3)](#valid3)\
**Vectors:** see polygon vectors above.\
**Focal length valid range:** `[ 100.0 - 3000.0 ]`.&ensp;[(3)](#valid3)\
**Near epsilon valid range:** `[ 0.5 - 3.0 ]`.&ensp;[(3)](#valid3)

#### Runtime

`runtime` is an **optional JSON object** setting the runtime properties.\
All its fields are optional overrides.

| Field | JSON type | Description |
|---|---|---|
| `loops` | Integer | Number of loops; `-1` means endless (default: `-1`).|
| `loop_delay` | Unsigned integer | Delay between frames, in milliseconds (default: `20`).|

**Loops valid range:** `[ -1 - 100 ]`.&ensp;[(3)](#valid3)\
**Loop delay valid range:** `[ 1 - 200 ]`.&ensp;[(3)](#valid3)

## JSON configuration example

See [`config/template.json`](./config/template.json).

```json
{
	"encoding": "UTF-8",

	"version": [ 0, 0, 3 ],

	"library":
	{
		"polygons": [
			{
				"name": "c_front",
				"color_idx": 4,
				"force": false,
				"vectors": [
					[ 100.0, 100.0, -100.0 ],
					[ 100.0, -100.0, -100.0 ],
					[ -100.0, -100.0, -100.0 ],
					[ -100.0, 100.0, -100.0 ]
				]
			}
		],
		"meshes": [
			{
				"name": "cube_mesh",
				"polygons": [ "c_front" ]
			}
		],
		"elements": [
			{
				"name": "cube_body",
				"mesh": "cube_mesh"
			}
		]
	},
	"scenes": [
		{
			"name": "cube",
			"elements": [
				{
					"name": "world",
					"parent": null,
					"active": true
				},
				{
					"name": "cube_1",
					"parent": "world",
					"active": true,
					"ref": "cube_body",
					"origin": [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 ],
					"motion": [ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 ]
				}
			],
			"environment": {
				"resolution": [ 1280, 800 ],
				"color_idx": 0,
				"light_type": 0,
				"light_direction": [ 0.0, 0.0, 1.0 ],
				"light_position": [ 0.0, 20.0, 75.0 ],
				"camera_position": [ 0.0, 0.0, 1000.0 ],
				"focal_len": 2000.0,
				"near_eps": 1.0,
			},
			"runtime": {
				"loops": -1,
				"loop_delay": 20
			}
		}
	]
}
```
