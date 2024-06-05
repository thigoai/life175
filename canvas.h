#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include "common.h"

class Canvas {
public:
    Canvas(int width, int height);
    void setPixel(int x, int y, Color color);
    void print() const;

    int Height() const
    {
        return height;
    }

    int Width() const
    {
        return width;
    }

    const Color* Pixels() const
    {
        return image.data();
    }

    std::vector<unsigned char> getPixelData() const; 

private:
    int width, height;
    std::vector<Color> image;  // Armazenar a imagem como uma sequência de cores
};

#endif // CANVAS_H
