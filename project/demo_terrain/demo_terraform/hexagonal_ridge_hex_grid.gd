extends HexagonalRidgeHexGrid

enum Biome {PLAIN = 0, HILL, MOUNTAIN, WATER}

var terraformer = Terraformer.new()

func _ready() -> void:
	set_terraformer(terraformer)


func _process(_delta: float) -> void:
	pass


func _on_button_pressed() -> void:
	set_biomes("PPPPPPPPPPPPPPPPPPP", 2)


func _on_mode_plain_button_pressed() -> void:
	terraformer.set_biome_to_set(Biome.PLAIN)


func _on_mode_hill_button_pressed() -> void:
	terraformer.set_biome_to_set(Biome.HILL)


func _on_mode_mountain_button_pressed() -> void:
	terraformer.set_biome_to_set(Biome.MOUNTAIN)


func _on_mode_water_button_pressed() -> void:
	terraformer.set_biome_to_set(Biome.WATER)
