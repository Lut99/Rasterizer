/* FRAGMENT V 1.glsl
 *   by Lut99
 *
 * Created:
 *   28/06/2021, 17:20:47
 * Last edited:
 *   28/06/2021, 17:20:47
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The implementation for our very simple fragment shader, which takes
 *   the colours outputted by the vertex shader and basically passes them
 *   on.
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
