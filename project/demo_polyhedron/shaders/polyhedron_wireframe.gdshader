shader_type spatial;

render_mode wireframe, unshaded;

void vertex() {
        VERTEX.y +=0.0;
}

void fragment() {
  ALBEDO = vec3(1,1,1);
}