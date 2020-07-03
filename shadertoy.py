import re
from PIL import Image
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

def loadTexture(tex, type_, fname):
    if type_ == 'Cube':
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex)
        name, ext = fname.split('.')
        for i in range(6):
            if i != 0:
                fname = name + '_{}.'.format(i) + ext
            im = Image.open(fname)
            if im.mode == "RGB":
                format = GL_RGB
            elif im.mode == "RGBA":
                format = GL_RGBA
            else:
                raise RuntimeError("unknown format")
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
                im.size[0], im.size[1], 0, format, GL_UNSIGNED_BYTE, im.tobytes())
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE)
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
    elif type_ == '2D':
        glBindTexture(GL_TEXTURE_2D, tex)
        im = Image.open(fname)
        if im.mode == "RGB":
            format = GL_RGB
        elif im.mode == "RGBA":
            format = GL_RGBA
        else:
            raise RuntimeError("unknown format")
        glTexImage2D(GL_TEXTURE_2D, 0, format, im.size[0], im.size[1],
            0, format, GL_UNSIGNED_BYTE, im.tobytes())
        glGenerateMipmap(GL_TEXTURE_2D)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
    else:
        raise RuntimeError("unknown texture type")

class Shadertoy():
    texture_pat = re.compile(r'//\s*\[shadertoy\]\s*sampler(Cube|2D)\s+"(.*)"\s*\n')
    def __init__(self, src):
        # preprocess
        tl = []
        def f(m):
            r = "uniform sampler{} iChannel{}; // {}\n".format(m.group(1), len(tl), m.group(2))
            tl.append(m.groups())
            return r
        src = re.sub(Shadertoy.texture_pat, f, src)
        # compile
        self.program = glCreateProgram()
        self.frag = glCreateShader(GL_FRAGMENT_SHADER)
        self.compile(src)
        glUseProgram(self.program)
        # load texture
        self.texture = glGenTextures(len(tl))
        for i, info in enumerate(tl):
            type_, fname = info
            glActiveTexture(GL_TEXTURE0 + i)
            loadTexture(self.texture[i], type_, fname)
            glUniform1i(glGetUniformLocation(self.program, "iChannel{}".format(i)), i);
    def __del__(self):
        glDeleteProgram(self.program)
        glDeleteShader(self.frag)
        glDeleteTextures(self.texture)
    def draw(self, res, time=0, mouse=(0,0,0,0)):
        glUniform3fv(glGetUniformLocation(self.program, "iResolution"), 1, res)
        glUniform1f(glGetUniformLocation(self.program, "iTime"), time)
        glUniform4fv(glGetUniformLocation(self.program, "iMouse"), 1, mouse)
        glRecti(-1,-1,1,1)
    def compile(self, src):
        frag_src = shadertoy_header + src
        glShaderSource(self.frag, frag_src)
        glCompileShader(self.frag)
        if not glGetShaderiv(self.frag, GL_COMPILE_STATUS):
            raise RuntimeError("Compiling Shader:\n"+glGetShaderInfoLog(self.frag).decode())
        glAttachShader(self.program, self.frag)
        glLinkProgram(self.program)
        glDetachShader(self.program, self.frag)

if __name__ == '__main__':
    import argparse
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

    st = Shadertoy(shadertoy_src)

    if args.offline:
        fps = args.fps
        from glapp import FFMpegVideoWriter
        iw = FFMpegVideoWriter("output.mp4", fps)
        for frame in range(int(args.time * fps)):
            st.draw((res[0], res[1], 1), frame/fps)
            glFinish()
            im = glReadPixels(0,0,res[0],res[1], GL_RGB,GL_UNSIGNED_BYTE)
            im = Image.frombytes("RGB", res, im).transpose(Image.FLIP_TOP_BOTTOM)
            iw.writeframe(im)
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
    del st
