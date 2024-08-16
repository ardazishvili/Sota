shader_type spatial;

//render_mode wireframe;

uniform sampler2D water_texture : source_color;
uniform sampler2D plain_texture : source_color;
uniform sampler2D hill_texture : source_color;
uniform sampler2D mountain_texture : source_color;

uniform float top_offset;
uniform float bottom_offset;
uniform float hill_level_ratio;

void vertex() {
        VERTEX.y += 0.0;
}

void fragment() {
  //float min_y = -0.07;
  float amplitude = abs(top_offset) + abs(bottom_offset);
  //float middle_border = 0.7;
  float first_coef = 1.0 / hill_level_ratio;
  float second_coef = 1.0 / (1.0 - hill_level_ratio);

  vec3 world_vertex = (INV_VIEW_MATRIX * vec4(VERTEX, 1.0)).xyz;
  float h = (world_vertex.y - bottom_offset) / amplitude;
  if (world_vertex.y < 0.0) {
    ALBEDO = texture(water_texture, UV).rgb;
  } else if (h <= hill_level_ratio) {
    ALBEDO = mix(texture(plain_texture, UV), texture(hill_texture, UV), h * first_coef).rgb;
  } else {
    ALBEDO = mix(texture(hill_texture, UV), texture(mountain_texture, UV), (h - hill_level_ratio) * second_coef).rgb;
  }
}

//void light() {
        // Called for every pixel for every light affecting the material.
        // Uncomment to replace the default light processing function with this one.
//}