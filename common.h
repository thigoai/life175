#ifndef COMMON_H
#define COMMON_H
#include <cstdint>
#include "lodepng.h"

// Estrutura para representar uma cor RGB
struct Color {
    uint8_t r, g, b, a;
};
#endif // COMMON_H