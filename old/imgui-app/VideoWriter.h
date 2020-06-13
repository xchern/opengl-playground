#pragma once

#include <stdio.h>

class VideoWriter {
public:
    VideoWriter(); // default to "output.mp4" with 24fps
    VideoWriter(const char * filename, int framerate);
    ~VideoWriter();
    void writeFrameRGB(int w, int h, float * p);
    void writeFrameRGB(int w, int h, unsigned char * p);
    void writeFrameGL(int w, int h);
    int getFrame() { return frame; }
private:
    int frame;
    FILE * fp;
};
