#include "canvas.h"
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <filesystem>


Canvas::Canvas(int width, int height) : width(width), height(height), image(width * height, {255, 255, 255}) {
    // Inicializa a imagem com branco
}

void Canvas::setPixel(int x, int y, Color color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Pixel position out of range");
    }
    image[y * width + x] = color;
}

void Canvas::print() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Color& color = image[y * width + x];
            // Usar escape ANSI para colorir o fundo
            std::cout << "\033[48;2;" << static_cast<int>(color.r) << ";"
                      << static_cast<int>(color.g) << ";"
                      << static_cast<int>(color.b) << "m  \033[0m";
        }
        std::cout << std::endl;
    }
}

std::vector<unsigned char> Canvas::getPixelData() const
{
    std::vector<unsigned char> pixelData(width * height * 4); // 4 bytes por pixel (RGBA)

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const Color& color = image[y * width + x];
            size_t index = (y * width + x) * 4;
            pixelData[index] = color.r;
            pixelData[index + 1] = color.g;
            pixelData[index + 2] = color.b;
            pixelData[index + 3] = color.a;
        }
    }

    return pixelData;
}
