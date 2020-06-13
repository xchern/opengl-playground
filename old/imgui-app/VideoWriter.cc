#include "VideoWriter.h"
#include <math.h>
#include <vector>
#include <GL/gl3w.h>

VideoWriter::VideoWriter() : VideoWriter("output.mp4", 24) {}

VideoWriter::VideoWriter(const char * filename, int framerate) : frame(0) {
    char cmd[256];
    sprintf(cmd,
            "ffmpeg -loglevel error -y "
            "-framerate %d -format image2pipe -i - "
            "-vf format=yuv420p \'%s\'",
            framerate, filename);
    fp = _popen(cmd, "w");
}

VideoWriter::~VideoWriter() {
    _pclose(fp);
}

inline
unsigned char uchar_of_float(float f) {
    int i = f * 256;
    if (i < 0) return 0;
    if (i > 255) return 255;
    return i;
}

void VideoWriter::writeFrameRGB(int w, int h, unsigned char * buf) {
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", w, h);
    fprintf(fp, "255\n");
    for (int y = h-1; y >= 0; y--)
        fwrite(&buf[y * w * 3], 3 * sizeof(unsigned char), w, fp);
    fflush(fp);
    frame++;
}

void VideoWriter::writeFrameRGB(int w, int h, float * buf) {
    std::vector<unsigned char> pixels(w * h * 3);
    for (int i = 0; i < w * h * 3; i++)
        pixels[i] = uchar_of_float(buf[i]);
    writeFrameRGB(w, h, pixels.data());
}

void VideoWriter::writeFrameGL(int w, int h) {
    std::vector<unsigned char> buf(w * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buf.data());
    writeFrameRGB(w, h, buf.data());
}
