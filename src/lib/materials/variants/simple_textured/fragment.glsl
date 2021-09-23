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
 *   The fragment shader for the SimpleTextured material. Doesn't apply any
 *   lighting, but instead returns the texture pixels associated with the with
 *   texel.
**/

#version 450

/* Memory layout */
// The input is the texel we get from the vertex shader
layout(location = 0) in vec2 frag_texel;
// The output is the color to actually render
layout(location = 0) out vec4 out_color;

// The image sampler for the texture
layout(set = 1, binding = 1) uniform sampler2D texture_sampler;



/* Entry point */
void main() {
    // Simply pass on
    out_color = texture(texture_sampler, frag_texel);
}
