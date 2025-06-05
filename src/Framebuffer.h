#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#include <limits>

struct Pixel {
    float r, g, b;
    float depth;
    
    Pixel() : r(0.0f), g(0.0f), b(0.0f), depth(std::numeric_limits<float>::max()) {}
    Pixel(float _r, float _g, float _b, float _d = std::numeric_limits<float>::max()) 
        : r(_r), g(_g), b(_b), depth(_d) {}
};

class Framebuffer 
{
private:
    int largura, altura;
    std::vector<std::vector<Pixel>> buffer;
    
public:
    Framebuffer(int w, int h);
    
    void limpar(float r = 0.0f, float g = 0.0f, float b = 0.0f);
    bool setPixel(int x, int y, float r, float g, float b, float depth);
    Pixel getPixel(int x, int y) const;
    
    void desenharNaTela(int offsetX, int offsetY);
    
    int getLargura() const { return largura; }
    int getAltura() const { return altura; }
    
    bool dentroDoBuffer(int x, int y) const;
};

#endif
