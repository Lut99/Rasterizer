/* FRAGMENT V 2.glsl
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
 *   on. Also has texture support.
**/

#version 450

/* Memory layout */
// The input is the color we get from the vertex shader
layout(location = 0) in vec3 frag_color;
// And the texture coordinate we got
layout(location = 1) in vec2 frag_texel;

// The output is the color to actually render
layout(location = 0) out vec4 out_color;

// The image sampler for the texture
layout(binding = 0) uniform sampler2D texture_sampler;



/* Entry point */
void main() {
    // Simply pass on
    out_color = texture(texture_sampler, frag_texel);
}
