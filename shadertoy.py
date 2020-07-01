from OpenGL.GL import *

shadertoy_header = '''
#version 330

uniform vec3      iResolution;
uniform float     iTime;
uniform vec4      iMouse;

//uniform float     iChannelTime[4];
//uniform vec4      iDate;
//uniform float     iSampleRate;
//uniform vec3      iChannelResolution[4];
//uniform int       iFrame;
//uniform float     iTimeDelta;
//uniform float     iFrameRate;

// TODO: textures 
/* uniform sampler2D iChannel%d; */
/* uniform samplerCube iChannel%d; */
/* uniform sampler3D iChannel%d; */

void mainImage( out vec4 c,  in vec2 f );

out vec4 outColor;
void main( void ) {
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    mainImage( color, gl_FragCoord.xy );
    outColor = color;
}
'''

class Shadertoy():
    def __init__(self):
        self.program = glCreateProgram()
        self.frag = glCreateShader(GL_FRAGMENT_SHADER)

    def __del__(self):
        glDeleteProgram(self.program)
        glDeleteShader(self.frag)

    def compile(self, src):
        frag_src = shadertoy_header + src
        glShaderSource(self.frag, frag_src)
        glCompileShader(self.frag)
        if not glGetShaderiv(self.frag, GL_COMPILE_STATUS):
            raise RuntimeError("Compiling Shader:\n"+glGetShaderInfoLog(self.frag).decode())
        glAttachShader(self.program, self.frag)
        glLinkProgram(self.program)
        glDetachShader(self.program, self.frag)

    def draw(self, res, time=0, mouse=(0,0,0,0)):
        glUseProgram(self.program)
        glUniform3fv(glGetUniformLocation(self.program, "iResolution"), 1, res)
        glUniform1f(glGetUniformLocation(self.program, "iTime"), time)
        glUniform4fv(glGetUniformLocation(self.program, "iMouse"), 1, mouse)
        glRecti(-1,-1,1,1)

if __name__ == '__main__':
    import argparse
    import sys
    import atexit
    from glapp import App

    parser = argparse.ArgumentParser()
    parser.add_argument('-res', default='960x720', type=str)
    parser.add_argument('-input')
    parser.add_argument('-offline', action='store_true')
    parser.add_argument('-fps', default=24, type=float)
    parser.add_argument('-time', default=10, type=float)
    args = parser.parse_args()

    if args.input:
        with open(args.input) as fp:
            shadertoy_src = fp.read()
    else:
        shadertoy_src = '''
void mainImage(out vec4 c, in vec2 f) {
    c = vec4(0,0,0,0);
    c.rgb += vec3(f/iResolution.xy, 0.5*(1.+sin(iTime)));
    c.rgb *= 1. - 1. / (1. + 0.2*length(iMouse.xy - f));
}
'''
    res = tuple(map(int, args.res.split('x')))
    App.init(res)

    st = Shadertoy()
    st.compile(shadertoy_src)

    if args.offline:
        fps = args.fps
        from glapp import FFMpegVideoWriter
        iw = FFMpegVideoWriter("output.mp4", fps)
        for frame in range(int(args.time * fps)):
            st.draw((res[0], res[1], 1), frame/fps)
            glFinish()
            iw.writeframe(0,0,res[0],res[1])
        del iw
                
    else:
        time0 = 0
        def display():
            w, h = App.res
            time = (App.time - time0)/1000
            flag = 1 if App.mouse_click[0] else -1
            x, y = App.mouse_pos
            xp, yp = App.mouse_pos_prev
            mouse = x, h - y, flag * xp, flag * (h - yp)
            st.draw((w, h, 1), time, mouse)
            App.title = "ShaderToy {:.2f} s".format(time)
        def keyboard(k):
            if k == b'\033':
                App.quit()
            elif k == b'r':
                global time0
                time0 = App.time
        App.display_cb = display
        App.keyboard_cb = keyboard
        App.run()
