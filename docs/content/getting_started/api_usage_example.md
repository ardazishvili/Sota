API usage
=========

This document contains usage examples of public Sota interface available for GDScript. Generally, addition of method done via binding e.g. `ClassDB::bind_method(D_METHOD("get_hex_meshes"), &HexGridMap::get_hex_meshes)`. After rebuild of C++ and reload of Godot editor (to use updated .so libraries) `get_hex_meshes` method will be available in editor.

Related code located in `/demo_api/scripts` folder of project

## Terrain examples

### Modify vertices of mesh
Let's assume that we have grid e.g. 5x5 of hexagon meshes and want to modify heights:
![terrain_api_0_before](/pics/howtos/api_terrain_0_before.png)

Following code aims to modify vertices based on noise values:
```gdscript
@tool

extends RectHexGridMap

@export var noise : FastNoiseLite

func get_heightmap_from_noise(hex : HexMesh, noise : FastNoiseLite):
	var result : PackedVector3Array
	var vertices = hex.get_vertices()
	for v : Vector3 in vertices:
		result.append(Vector3(v.x, noise.get_noise_2d(v.x, v.z), v.z))
	return result

func _ready() -> void:
	for hex : SotaMesh in get_hex_meshes():
		hex.set_vertices(get_heightmap_from_noise(hex, noise))

func _process(delta: float) -> void:
	pass
```
Produced mesh:
![terrain_api_0_before](/pics/howtos/api_terrain_0_after.png)
