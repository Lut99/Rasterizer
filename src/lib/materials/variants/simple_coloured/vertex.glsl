/* VERTEX.glsl
 *   by Lut99
 *
 * Created:
 *   20/09/2021, 14:42:44
 * Last edited:
 *   20/09/2021, 14:42:44
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Vertex shader for the SimpleColoured material. Doesn't do anything with
 *   textures yet, so only returns the vertex' colour to the fragment shader.
**/

#version 450

/* Memory layout */
// We take a list of 3D positions of the points
layout(location = 0) in vec3 vertex;
// We drop the color of the vertex for the fragment shader
layout(location = 0) out vec3 frag_color;

// The camera data as a uniform buffer
layout(set = 0, binding = 0) uniform Camera {
    mat4 proj;
    mat4 view;
} camera;
// The material data as a uniform buffer
layout(set = 1, binding = 0) uniform Material {
    vec3 color;
} material;
// The object data as a uniform buffer
layout(set = 2, binding = 0) uniform Object {
    mat4 translation;
} object;



/* Entry point */
void main() {
    // Return the vertex as a 4D vertex
    gl_Position = camera.proj * camera.view * object.translation * vec4(vertex, 1.0);
    // Also return the color for the fragment shader
    frag_color = material.color;
}
