shader_type spatial;

uniform sampler2D water_texture : source_color;
uniform sampler2D plain_texture : source_color;
uniform sampler2D hill_texture : source_color;
uniform sampler2D mountain_texture : source_color;

uniform float water_height;
uniform float plain_height;
uniform float hill_height;
uniform float mountain_height;

void vertex() {
        VERTEX.y += 0.0;
}

const float alt_step = 0.0001;

void fragment() {
  float polyhedron_R = 1.0; // hardcode
  float world_vertex_relative_to_r = length((INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz) - polyhedron_R;
  if (world_vertex_relative_to_r < (water_height + alt_step)) {
    ALBEDO = texture(water_texture, UV).rgb;
  } else if ((water_height + alt_step) <= world_vertex_relative_to_r && world_vertex_relative_to_r < (plain_height + alt_step) ) {
	ALBEDO = texture(plain_texture, UV).rgb;
  } else if ((plain_height + alt_step) <= world_vertex_relative_to_r && world_vertex_relative_to_r < (hill_height + alt_step)) {
	ALBEDO = texture(hill_texture, UV).rgb;
  } else if (world_vertex_relative_to_r >= (hill_height + alt_step)){
	ALBEDO = texture(mountain_texture, UV).rgb;
  }
}