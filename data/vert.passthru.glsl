/**
 * Vertex shader that does nothing
 * 
 * :texcoord0: variable passed to the fragment shader
 * :texdim0: variable passed to the fragment shader
 */
varying vec2 texcoord0;
varying vec2 texdim0;

void main()
{
    gl_Position = ftransform();
    texcoord0 = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
    texdim0 = vec2(abs(gl_TextureMatrix[0][0][0]), abs(gl_TextureMatrix[0][1][1]));
}
