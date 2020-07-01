from OpenGL.GL import *
from OpenGL.GLUT import *

class App():
    _window = None
    _interval = 0 # time consumption of each frame
    time = 0 # current time
    mouse_click = [0,]
    mouse_pos = [0, 0]
    mouse_pos_prev = [0, 0]
    res = [0, 0]
    title = "OpenGL App"
    display_cb = lambda: None
    keyboard_cb = lambda k: None

    @staticmethod
    def init(res=None):
        glutInit([])
        if res:
            glutInitWindowSize(*res)
        App._window = glutCreateWindow(b"")

        print(glGetString(GL_VENDOR).decode(), glGetString(GL_RENDERER).decode())
        print("OpenGL Version:", glGetString(GL_VERSION).decode())
        print("GLSL Version:", glGetString(GL_SHADING_LANGUAGE_VERSION).decode())
        
        glutDisplayFunc(App.display)
        glutMouseFunc(App.mouse)
        glutMotionFunc(App.motion)
        glutPassiveMotionFunc(App.motion)
        glutKeyboardFunc(App.keyboard)
        
        glClearColor(1,1,1,1)
        glColor3f(0,0,0)

    @staticmethod
    def quit():
        glutDestroyWindow(App._window)
        exit

    @staticmethod
    def run():
        glutMainLoop()

    @staticmethod
    def display():
        time = glutGet(GLUT_ELAPSED_TIME)
        App._interval += (time - App.time - App._interval) * 5e-2
        App.time = time
        
        App.res[0] = glutGet(GLUT_WINDOW_WIDTH)
        App.res[1] = glutGet(GLUT_WINDOW_HEIGHT)

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        App.display_cb()

        glutSetWindowTitle("{} ({:.1f} ms/f)".format(App.title, App._interval))
        glutSwapBuffers()
        glutPostRedisplay()

    @staticmethod
    def mouse(button, state, x, y):
        if button == GLUT_LEFT_BUTTON:
            if state == GLUT_DOWN:
                App.mouse_pos_prev[0], App.mouse_pos_prev[1] = x, y
                App.mouse_pos[0], App.mouse_pos[1] = x, y
                App.mouse_click[0] = 1
            else:
                App.mouse_click[0] = 0

    @staticmethod
    def motion(x, y):
        App.mouse_pos[0], App.mouse_pos[1] = x, y
    
    @staticmethod
    def keyboard(k, x, y):
        App.mouse_pos[0], App.mouse_pos[1] = x, y
        App.keyboard_cb(k)

class FFMpegVideoWriter():
    def __init__(self, file, fps):
        if file[-4:] != '.mp4':
            file += '.mp4'
        import subprocess
        cmd = ("ffmpeg -framerate {fps} -f image2pipe "
        "-i - -vf format=yuv420p -y {file}").format(fps=fps, file=file)
        self.pipe = subprocess.Popen(cmd, stdin=subprocess.PIPE)
        
    def writeframe(self, x, y, width, height):
        from PIL import Image
        im = glReadPixels(x, y, width, height, GL_RGB,GL_UNSIGNED_BYTE)
        im = Image.frombytes("RGB", (width, height), im).transpose(Image.FLIP_TOP_BOTTOM)
        im.save(self.pipe.stdin, "PNG")

if __name__ == '__main__':
    App.init()
    iw = FFMpegVideoWriter("output.mp4", 60)
    def display():
        glLoadIdentity()
        glRotate(App.time*120/1000,0,0,1)
        glBegin(GL_TRIANGLES)
        glColor3f(1,0,0)
        glVertex2f(1,0)
        glColor3f(0,1,0)
        glVertex2f(-0.5,-0.8)
        glColor3f(0,0,1)
        glVertex2f(-0.5,0.8)
        glEnd()
        glFinish()
        iw.writeframe(0,0,App.res[0],App.res[1])
    App.display_cb = display
    App.run()