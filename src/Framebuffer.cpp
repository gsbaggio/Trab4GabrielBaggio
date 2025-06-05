#include "Framebuffer.h"
#include "gl_canvas2d.h"
#include <algorithm>

Framebuffer::Framebuffer(int w, int h) : largura(w), altura(h)
{
    buffer.resize(altura);
    for (int i = 0; i < altura; i++) {
        buffer[i].resize(largura);
    }
}

void Framebuffer::limpar(float r, float g, float b)
{
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            buffer[y][x] = Pixel(r, g, b);
        }
    }
}

bool Framebuffer::setPixel(int x, int y, float r, float g, float b, float depth)
{
    if (!dentroDoBuffer(x, y)) {
        return false;
    }
    
    // Teste Z-buffer
    if (depth < buffer[y][x].depth) {
        buffer[y][x] = Pixel(r, g, b, depth);
        return true;
    }
    
    return false;
}

Pixel Framebuffer::getPixel(int x, int y) const
{
    if (dentroDoBuffer(x, y)) {
        return buffer[y][x];
    }
    return Pixel();
}

void Framebuffer::desenharNaTela(int offsetX, int offsetY)
{
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            Pixel& p = buffer[y][x];
            CV::color(p.r, p.g, p.b);
            CV::point(offsetX + x, offsetY + y);
        }
    }
}

bool Framebuffer::dentroDoBuffer(int x, int y) const
{
    return x >= 0 && x < largura && y >= 0 && y < altura;
}
