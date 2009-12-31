#!/usr/bin/env python
"""
GLSL shaders with SDL, OpenGL texture and Python
"""
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.error import GLError
from OpenGL.GLU import *
from rats.glsl import ShaderProgram
from rats.glsl import ShaderError
from toon import fx
from toon import optgroup

# Vertex shader that does nothing
#
# :texcoord0: variable passed to the fragment shader
# :texdim0: variable passed to the fragment shader
vert = """
varying vec2 texcoord0;
varying vec2 texdim0;
void main()
{
    gl_Position = ftransform();
    texcoord0 = vec2(gl_TextureMatrix[0] * gl_MultiTexCoord0);
    texdim0 = vec2(abs(gl_TextureMatrix[0][0][0]), abs(gl_TextureMatrix[0][1][1]));
}
"""
# fragment shader for adjusting brightness, contrast and saturation
frag = """
/** 
 * GLSL fragment shader that desaturates the image, and mix it back 
 * with the original image in overlay mode, to create a nice cine-like effect.
 */
#extension GL_ARB_texture_rectangle : enable
// user configurable variables (read-only here)
varying vec2 texcoord0;
varying vec2 texdim0;
// the texture
uniform sampler2DRect image;
// arguments
uniform vec3 avgluma;
uniform float saturation;
uniform float contrast;
uniform float brightness;
uniform float alpha;
// constants
const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
const vec3 white = vec3(1.0, 1.0, 1.0);
void main (void)
{
    float input_alpha = gl_Color.a;
	vec3 texColor = texture2DRect(image, texcoord0).rgb;
	float luminance = dot(texColor, LumCoeff);
	vec3 color = mix(vec3(luminance), texColor, saturation);
	color = mix(avgluma, color, contrast);
	color *= brightness;
	                // What was that ?? gl_FragColor = vec4(color, color.g*alpha);
    // Next, we apply it in overlay over the original image !
    // color on top of texColor in overlay:
    if (luminance < 0.45) 
    {
        color = 2.0 * color * texColor;
    } 
    else if (luminance > 0.55) 
    {
        color = white - 2.0 * (white - color) * (white - texColor);
    }
    else
    {
        vec3 result1 = 2.0 * color * texColor;
        vec3 result2 = white - 2.0 * (white - color) * (white - texColor);
        color = mix(result1, result2, (luminance - 0.45) * 10.0);
    }
    color = mix(color, texColor, 0.5); // reinject some of the original image in it.
	gl_FragColor = vec4(color, alpha * input_alpha);
}
"""
class Options(optgroup.OptionsGroup):
    def __init__(self):
        self.avgluma = [1.0, 1.0, 1.0]
        self.saturation = 0.0
        self.contrast = 1.0
        self.brightness = 1.0
        self.alpha = 1.0
        self.texture_id = 0
    
class Effect(fx.Effect):
    def __init__(self):
        fx.Effect.__init__(self)
        self.program = None
        self.options = Options()
        self.name = "cinelike"
        #print("init %s" % (self.name))
        
    def setup(self):
        global vert
        global frag
        if True:
        #try:
            self.program = ShaderProgram()
            self.program.add_shader_text(GL_VERTEX_SHADER_ARB, vert)
            self.program.add_shader_text(GL_FRAGMENT_SHADER_ARB, frag)
            self.program.linkShaders()
        #except Exception, e: 
        #    print(e.message)
        #else:
            self.loaded = True
    
    def pre_draw(self):
        if self.enabled and self.loaded:
            try:
                self.program.enable()
            except Exception, e: 
                print(e.message)
            self.program.glUniform1i("image", self.options.texture_id)
            self.program.glUniform1f("saturation", self.options.saturation)
            self.program.glUniform1f("contrast", self.options.contrast)
            self.program.glUniform1f("brightness", self.options.brightness)
            self.program.glUniform1f("alpha", self.options.alpha)
            self.program.glUniform3f("avgluma", *self.options.avgluma)

    def post_draw(self):
        if self.enabled and self.loaded:
            try:
                self.program.disable()
            except Exception, e: 
                print e.message

def create_effect():
    return Effect()
