shader_type spatial;

render_mode wireframe, unshaded;

void vertex() {
        VERTEX.y +=0.0;
}

void fragment() {
  ALBEDO = vec3(1,1,1);
}

//void light() {
        // Called for every pixel for every light affecting the material.
        // Uncomment to replace the default light processing function with this one.
//}