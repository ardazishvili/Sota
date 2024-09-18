shader_type spatial;

uniform sampler2D water_texture : source_color;
uniform sampler2D plain_texture : source_color;
uniform sampler2D hill_texture : source_color;
uniform sampler2D mountain_texture : source_color;

uniform float water_height;
uniform float plain_height;
uniform float hill_height;

void vertex() {
        VERTEX.y += 0.0;
}

void fragment() {
  float polyhedron_R = 1.0; // hardcode
  float world_vertex_relative_to_r = length((INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz) - polyhedron_R;
  if (world_vertex_relative_to_r < water_height) {
    ALBEDO = texture(water_texture, UV).rgb;
  } else if (water_height <= world_vertex_relative_to_r && world_vertex_relative_to_r < plain_height) {
	ALBEDO = texture(plain_texture, UV).rgb;
  } else if (plain_height <= world_vertex_relative_to_r && world_vertex_relative_to_r < hill_height ) {
	ALBEDO = texture(hill_texture, UV).rgb;
  } else if (world_vertex_relative_to_r >= hill_height){
	ALBEDO = texture(mountain_texture, UV).rgb;
  }
}