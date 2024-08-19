shader_type spatial;

//render_mode wireframe;

uniform sampler2D cell_texture : source_color;

void vertex() {
        VERTEX.y += 0.0;
}

void fragment() {
  ALBEDO = texture(cell_texture, UV).rgb;
}