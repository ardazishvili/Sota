shader_type spatial;

uniform float water_height = 0.03;
uniform float plain_height = 0.07;
uniform float mountain_height = 0.13;

const vec3 shore_color = vec3(251, 251, 255) / 255.0;
const vec3 shore_top_color = vec3(131, 142, 131) / 255.0;
const vec3 mountain_color = vec3(29, 32, 31) / 255.0;

const float polyhedron_R = 1.0; // hardcode

void vertex() {
    VERTEX.y += 0.0;
}

void fragment() {
  float world_vertex_relative_to_r = length((INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz) - polyhedron_R;
  if (world_vertex_relative_to_r < water_height) {
    ALBEDO = vec3(0.0, 0.0 ,1.0);
  } else if (water_height <= world_vertex_relative_to_r && world_vertex_relative_to_r < plain_height){
	ALBEDO = mix(shore_color, shore_top_color, (world_vertex_relative_to_r - water_height) / (plain_height - water_height));
  } else if (plain_height <= world_vertex_relative_to_r && world_vertex_relative_to_r < mountain_height) {
	ALBEDO = mix(shore_top_color, mountain_color, (world_vertex_relative_to_r - plain_height) / (mountain_height - plain_height));
  } else {
	ALBEDO = vec3(1.0, 0, 0);
  }
}