/* VERTEX V 1.glsl
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
 *   Code for our very simple vertex shader. Basically just passes on
 *   hardcoded vertices.
**/

#version 450

/* Define the layout of the memory; for us, we just output the colors for the fragment shader. */
layout(location = 0) out vec3 fragColor;

/* The list of hardcoded vertices we use. */
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

/* The list of hardcoded colours we use. */
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);



/* Entry point */
void main() {
    // Return the vertex as a 4D vertex
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    // Also return the hardcoded color for the fragment shader
    fragColor = colors[gl_VertexIndex];
}
