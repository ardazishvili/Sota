@tool

extends MeshInstance3D

class_name Polyhedron

@export var rows : int = 1

const EPSILON = 0.0001

var y_coord = 0.0
var top = Vector3(0, y_coord, sqrt(3) / 2)
var center = Vector3(0, y_coord, sqrt(3) / (2 * 3))
var left = Vector3(-0.5, y_coord, 0)
var right = Vector3(0.5, y_coord, 0)

func f1(x) -> float: return sqrt(3) * x + sqrt(3) / 2
func f2(x) -> float: return -sqrt(3) * x + sqrt(3) / 2

var vertices : PackedVector3Array
var normals : PackedVector3Array

var hexagons : Dictionary

func ico_points() -> PackedVector3Array:
	var s = 2 / sqrt(5)
	var c = 1 / sqrt(5)
	var res : PackedVector3Array
	res.append(Vector3(0, 1, 0))
	for i in 5:
		res.append(Vector3(s * cos(i * 2  * PI / 5), c, s * sin(i * 2 * PI / 5)))
	for i in 6:
		var v = res[i]
		res.append(Vector3(-v.x, -v.y, v.z))
	return res

func ico_indices() -> Array[Vector3i]:
	var res : Array[Vector3i]
	for i in 5:
		res.append(Vector3i((i + 1) % 5 + 1, 0, i + 1))
	for i in 5:
		res.append(Vector3i((i + 1) % 5 + 7, 6, i + 7))
	for i in 5:
		res.append(Vector3i(i + 1, (7 - i) % 5 + 7, (i + 1) % 5 + 1))
	for i in 5:
		res.append(Vector3i(i + 1, (8 - i) % 5 + 7, (7 - i) % 5 + 7))
	return res

func barycentric(point : Vector2) -> Vector3:
	var x = point.x
	var y = point.y
	var l3 = y * 2.0 / sqrt(3.0)
	var l2 = x + 0.5 * (1 - l3)
	var l1 = 1 - l2 - l3
	return Vector3(l1, l2, l3)

func map2d_to_3d(point : Vector2, s1 : Vector3, s2 : Vector3, s3 : Vector3):
	var bar_coords = barycentric(point)
	var l1 = bar_coords.x
	var l2 = bar_coords.y
	var l3 = bar_coords.z
	if abs(l3 - 1) < 1e-10:
		return s3
	var l2s = l2 / (l1 + l2)
	var p12 = s1.slerp(s2, l2s)
	return p12.slerp(s3, l3)

# based on answer https://stackoverflow.com/a/47043459
func get_vertices() -> PackedVector3Array:
	var full_hexes = 1
	var r = (1.0 / 2)/ (full_hexes + 1)
	
	var R = r * 2 / sqrt(3)
	var diameter = 2 * R
	var start_point = Vector3(-0.5, 0, 0)

	var key_step = r / 3.0
	
	print(r)
	
	var out_of_triangle = func(x, z) -> bool: return z < 0 || z > f1(x) || z > f2(x)
	var points : PackedVector3Array = ico_points()
	var indices : Array[Vector3i] = ico_indices()
	var final : PackedVector3Array
	for t in 20:
		var triangle : Vector3i = indices[t]
		for i in range(0, full_hexes + 2):
			for j in range(0, full_hexes + 2):
				if i == 0 && (j == 0 || j == (full_hexes + 1)):
					continue # first and last points are centers of pentagon
				if i == (full_hexes + 1):
					continue # last row is center of pentagon
				var hex_center = Vector3(start_point.x + 2 * r * j, 0, start_point.z + diameter * 3.0 * i / 4.0)
				if (i & 1):
					hex_center.x -= r # offset of odd rows of hexagons
				if (hex_center.z < -r/2) || hex_center.z > (f1(hex_center.x) + r/2) || hex_center.z > (f2(hex_center.x) + r /2):
					continue # skip hexagons out of sample triangle
				
				var mapped_center = map2d_to_3d(Vector2(hex_center.x, hex_center.z), points[triangle.x], points[triangle.y], points[triangle.z])
				var key = Vector3i(round(mapped_center.x / key_step), round(mapped_center.y / key_step), round(mapped_center.z / key_step)) # to use dict. due to float fluctuations
				var hex : Hexagon = hexagons.get_or_add(key, Hexagon.new())
				hex.center = mapped_center
				for angle in Vector3(-PI / 6, 11 * PI / 6, PI / 3):
					var point = Vector3(hex_center.x + cos(angle) * R, 0, hex_center.z + sin(angle) * R)
					if !out_of_triangle.call(point.x, point.z):
						hex.points.append(map2d_to_3d(Vector2(point.x, point.z), points[triangle.x], points[triangle.y], points[triangle.z]))
	
	for hexagon : Hexagon in hexagons.values():
		if hexagon.points.size() != 6:
			printerr("Hexagon has != 6 points")
			printerr(hexagons)
			printerr(hexagon)
			printerr(key_step)
			for k in hexagons.keys():
				print(k)
				print(hexagons[k].center)
				print(hexagons[k].points)
		var c = hexagon.center
		var v0 = hexagon.points[0] - c
		hexagon.points.sort_custom(func(a, b):
			var v1 : Vector3 = a - c
			var v2 : Vector3 = b - c
			return v1.signed_angle_to(v0, c) < v2.signed_angle_to(v0, c)
			)
		for i in 6:
			var a = hexagon.points[i]
			var b = hexagon.points[(i + 1) % 6]
			final.append(hexagon.center)
			final.append(a)
			final.append(b)
	return final

func get_normals():
	var res : PackedVector3Array
	for i in range(0, vertices.size(), 3):
		var v0 = vertices[i]
		var v1 = vertices[i + 1]
		var v2 = vertices[i + 2]
		var normal : Vector3 = (v0 - v1).cross(v2 - v1).normalized()
		res.append(normal)
		res.append(normal)
		res.append(normal)
	return res

func _ready() -> void:
	var surface_array = []
	surface_array.resize(Mesh.ARRAY_MAX)

	vertices = get_vertices()
	normals = get_normals()
	surface_array[Mesh.ARRAY_VERTEX] = vertices
	surface_array[Mesh.ARRAY_NORMAL] = normals

	mesh = ArrayMesh.new()
	#mesh.add_surface_from_arrays(Mesh.PRIMITIVE_LINES, surface_array)
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, surface_array)
