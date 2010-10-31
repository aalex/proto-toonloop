// test for alpha using fragment shaders.
#extension GL_ARB_texture_rectangle : enable
uniform float alpha;
// the texture
uniform sampler2D image;
void main(void)
{
    vec3 color = texture2D(image, gl_TexCoord[0].st).rgb;
    float input_alpha = gl_Color.a;
    gl_FragColor = vec4(color, alpha * input_alpha);
}
