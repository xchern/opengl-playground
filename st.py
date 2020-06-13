import argparse
import sys
import atexit
from OpenGL.GL import *
from OpenGL.GLUT import *

parser = argparse.ArgumentParser()
parser.add_argument('-res', default='960x720', type=str)
parser.add_argument('-offline', action='store_true')
parser.add_argument('file')
args = parser.parse_args()

class GLUTApp():
    _time, _interval = 0, 0
    _window = None
    mouse_click = [0,]
    mouse_pos = [0, 0]
    mouse_pos_prev = [0, 0]
    title = "OpenGL App"
    display_cb = lambda: None
    keyboard_cb = lambda k: None
    def display():
        time = glutGet(GLUT_ELAPSED_TIME)
        GLUTApp._interval += (time - GLUTApp._time - GLUTApp._interval) * 5e-2
        GLUTApp._time = time

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        GLUTApp.display_cb()

        glutSetWindowTitle("{} ({:.1f} ms/f)".format(GLUTApp.title, GLUTApp._interval))
        glutSwapBuffers()
        glutPostRedisplay()
    def mouse(button, state, x, y):
        if button == GLUT_LEFT_BUTTON:
            if state == GLUT_DOWN:
                GLUTApp.mouse_pos_prev[0], GLUTApp.mouse_pos_prev[1] = x, y
                GLUTApp.mouse_pos[0], GLUTApp.mouse_pos[1] = x, y
                GLUTApp.mouse_click[0] = 1
            else:
                GLUTApp.mouse_click[0] = 0
    def motion(x, y):
        GLUTApp.mouse_pos[0], GLUTApp.mouse_pos[1] = x, y
    def keyboard(k, x, y):
        GLUTApp.mouse_pos[0], GLUTApp.mouse_pos[1] = x, y
        GLUTApp.keyboard_cb(k)
    def init(res=None):
        glutInit([])
        if res:
            glutInitWindowSize(*res)
        GLUTApp._window = glutCreateWindow(b"")
        print(glGetString(GL_VENDOR).decode(), glGetString(GL_RENDERER).decode())
        print("OpenGL Version:", glGetString(GL_VERSION).decode())
        print("GLSL Version:", glGetString(GL_SHADING_LANGUAGE_VERSION).decode())
        
        glutDisplayFunc(GLUTApp.display)
        glutMouseFunc(GLUTApp.mouse)
        glutMotionFunc(GLUTApp.motion)
        glutPassiveMotionFunc(GLUTApp.motion)
        glutKeyboardFunc(GLUTApp.keyboard)
        
        glClearColor(1,1,1,1)
        glColor3f(0,0,0)
    def quit():
        glutDestroyWindow(GLUTApp._window)
        exit

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

shadertoy_src = '''
void mainImage(out vec4 c, in vec2 f) {
    c = vec4(f/iResolution.xy,0.5*(1+sin(iTime)),1);
}
'''

with open(args.file) as fp:
    shadertoy_src = fp.read()

GLUTApp.init(tuple(map(int, args.res.split('x'))))

program = glCreateProgram()
atexit.register(lambda: glDeleteProgram(program))

frag = glCreateShader(GL_FRAGMENT_SHADER)
frag_src = shadertoy_header + shadertoy_src
glShaderSource(frag, frag_src)
glCompileShader(frag)
if not glGetShaderiv(frag, GL_COMPILE_STATUS):
    raise RuntimeError("Compiling Shader:\n"+glGetShaderInfoLog(frag).decode())

glAttachShader(program, frag)
glLinkProgram(program)
glDetachShader(program, frag)
glDeleteShader(frag)

glUseProgram(program)

time0 = 0
def display():
    w, h = glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)
    t = (glutGet(GLUT_ELAPSED_TIME) - time0)/1000
    glUniform3f(glGetUniformLocation(program, "iResolution"), w, h, 1)
    glUniform1f(glGetUniformLocation(program, "iTime"), t)
    flag = 1 if GLUTApp.mouse_click[0] else -1
    x, y = GLUTApp.mouse_pos
    xp, yp = GLUTApp.mouse_pos_prev
    glUniform4f(glGetUniformLocation(program, "iMouse"),
            x, h - y, flag * xp, flag * (h - yp))
    glRecti(-1,-1,1,1)
    GLUTApp.title = "ShaderToy {:.2f} s".format(t)
def keyboard(k):
    if k == b'\033':
        GLUTApp.quit()
    elif k == b'r':
        global time0
        time0 = glutGet(GLUT_ELAPSED_TIME)
    elif k == b'f':
        glutFullScreen()
GLUTApp.display_cb = display
GLUTApp.keyboard_cb = keyboard

if args.offline:
    glutHideWindow()
    # TODO generate video
else:
    glutMainLoop()