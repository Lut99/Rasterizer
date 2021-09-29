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
 *   Vertex shader for the SimpleTextured material. Doesn't do anything with
 *   colour, but does pass the texel coordinate to the fragment shader.
**/

#version 450

/* Memory layout */
// We take a list of 3D positions of the points
layout(location = 0) in vec3 vertex;
// And a 3D color
layout(location = 1) in vec2 texel;

// We drop the color of the vertex for the fragment shader
layout(location = 0) out vec2 frag_texel;

// The camera data as a uniform buffer
layout(set = 0, binding = 0) uniform Camera {
    mat4 proj;
    mat4 view;
} camera;
// The object data as a uniform buffer
layout(set = 2, binding = 0) uniform Object {
    mat4 translation;
} object;



/* Entry point */
void main() {
    // Return the vertex as a 4D vertex
    gl_Position = camera.proj * camera.view * object.translation * vec4(vertex, 1.0);
    // Also return the color for the fragment shader
    frag_texel = texel;
}
