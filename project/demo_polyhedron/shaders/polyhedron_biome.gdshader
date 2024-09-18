shader_type spatial;

//render_mode wireframe, unshaded;

uniform sampler2D water_texture : source_color;
uniform sampler2D plain_texture : source_color;
uniform sampler2D hill_texture : source_color;
uniform sampler2D mountain_texture : source_color;

//uniform float top_offset;
//uniform float bottom_offset;
//uniform float hill_level_ratio;

uniform float top_offset_l = 0.2;
uniform float bottom_offset_l = -0.014;
uniform float hill_level_ratio_l = 0.18;

const float polyhedron_R = 1.0; // hardcode

void vertex() {
        VERTEX.y += 0.0;
}

void fragment() {
  float world_vertex_relative_to_r = length((INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz) - polyhedron_R;

  float amplitude = abs(top_offset_l) + abs(bottom_offset_l);
  float first_coef = 1.0 / hill_level_ratio_l;
  float second_coef = 1.0 / (1.0 - hill_level_ratio_l);

  float h = (world_vertex_relative_to_r - bottom_offset_l) / amplitude;
  if (world_vertex_relative_to_r < -0.075) {
    ALBEDO = texture(water_texture, UV).rgb;
  } else if (h <= hill_level_ratio_l) {
    ALBEDO = mix(texture(plain_texture, UV), texture(hill_texture, UV), h * first_coef).rgb;
  } else {
    ALBEDO = mix(texture(hill_texture, UV), texture(mountain_texture, UV), (h - hill_level_ratio_l) * second_coef).rgb;
  }
}