/* FRAGMENT.glsl
 *   by Lut99
 *
 * Created:
 *   20/09/2021, 14:44:05
 * Last edited:
 *   20/09/2021, 14:44:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The fragment shader for the SimpleColoured material. Doesn't apply any
 *   lighting, but instead returns the colour as given by the vertex
 *   shader.
**/

#version 450

/* Memory layout */
// The input is the color we get from the vertex shader
layout(location = 0) in vec3 fragColor;
// The output is the color to actually render
layout(location = 0) out vec4 outColor;



/* Entry point */
void main() {
    // Simply pass on
    outColor = vec4(fragColor, 1.0);
}
