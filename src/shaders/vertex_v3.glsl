/* VERTEX V 2.glsl
 *   by Lut99
 *
 * Created:
 *   28/06/2021, 17:18:50
 * Last edited:
 *   28/06/2021, 17:18:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Code for our very simple vertex shader. This version takes vertices
 *   dynamically from the input list of vertices.
**/

#version 450

/* Memory layout */
// We take a list of 2D positions of the points
layout(location = 0) in vec3 vertex;
// And a 3D color
layout(location = 1) in vec3 color;

// We drop the color of the vertex for the fragment shader
layout(location = 0) out vec3 frag_color;

// The camera data as push constants
layout(push_constant) uniform Camera {
    mat4 proj;
    mat4 view;
    mat4 model;
} camera;



/* Entry point */
void main() {
    // Return the vertex as a 4D vertex
    // gl_Position = camera.proj * camera.view * camera.model * vec4(vertex, 0.0, 1.0);
    gl_Position = camera.proj * camera.view * camera.model * vec4(vertex, 1.0);
    // Also return the color for the fragment shader
    frag_color = color;
}
