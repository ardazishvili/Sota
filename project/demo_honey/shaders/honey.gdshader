shader_type spatial;

//render_mode wireframe;

uniform sampler2D honey_texture : source_color;

void vertex() {
        VERTEX.y -= 0.0;
}

void fragment() {
  ALBEDO = texture(honey_texture, UV).rgb;
}