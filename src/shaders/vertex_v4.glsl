/* VERTEX V 4.glsl
 *   by Lut99
 *
 * Created:
 *   16/08/2021, 11:44:50
 * Last edited:
 *   16/08/2021, 11:44:50
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Code for our very simple vertex shader. This version takes vertices
 *   dynamically from the input list of vertices. Also has texture support.
**/

#version 450

/* Memory layout */
// We take a list of 3D positions of the points
layout(location = 0) in vec3 vertex;
// And a 3D color
layout(location = 1) in vec3 color;

// We drop the color of the vertex for the fragment shader
layout(location = 0) out vec3 frag_color;

// The camera data as push constants
layout(push_constant) uniform Camera {
    mat4 mat;
} camera;



/* Entry point */
void main() {
    // Return the vertex as a 4D vertex
    // gl_Position = camera.proj * camera.view * camera.model * vec4(vertex, 0.0, 1.0);
    gl_Position = camera.mat * vec4(vertex, 1.0);
    // Also return the color & normal for the fragment shader
    frag_color = color;
}
