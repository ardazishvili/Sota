@tool

extends RectHexGrid

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
