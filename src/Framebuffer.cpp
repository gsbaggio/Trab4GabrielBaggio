/*
implementa um framebuffer para renderização de gráficos 3D com z-buffer
gerencia uma matriz de pixels com teste de profundidade para determinar visibilidade
permite desenhar pixels individuais e renderizar o buffer completo na tela
*/

#include "Framebuffer.h"
#include "gl_canvas2d.h"
#include <algorithm>

// construtor que inicializa o framebuffer com dimensões especificadas
Framebuffer::Framebuffer(int w, int h) : largura(w), altura(h)
{
    // redimensiona o buffer para a altura especificada
    buffer.resize(altura);
    // inicializa cada linha do buffer com a largura especificada
    for (int i = 0; i < altura; i++) {
        buffer[i].resize(largura);
    }
}

// limpa todo o framebuffer com uma cor sólida
void Framebuffer::limpar(float r, float g, float b)
{
    // percorre todos os pixels do buffer
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            // define cada pixel com a cor especificada
            buffer[y][x] = Pixel(r, g, b);
        }
    }
}

// define um pixel no buffer com teste de profundidade z-buffer
bool Framebuffer::setPixel(int x, int y, float r, float g, float b, float depth)
{
    // verifica se as coordenadas estão dentro dos limites do buffer
    if (!dentroDoBuffer(x, y)) {
        return false;
    }
    
    // teste z-buffer - apenas desenha se o pixel está mais próximo
    if (depth < buffer[y][x].depth) {
        buffer[y][x] = Pixel(r, g, b, depth);
        return true;
    }
    
    return false;
}

// obtém a cor de um pixel no buffer
Pixel Framebuffer::getPixel(int x, int y) const
{
    // verifica se as coordenadas estão dentro dos limites do buffer
    if (dentroDoBuffer(x, y)) {
        return buffer[y][x];
    }
    return Pixel();
}

// renderiza todo o framebuffer na tela usando offset especificado
void Framebuffer::desenharNaTela(int offsetX, int offsetY)
{
    // percorre todos os pixels do buffer
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            // obtém referência do pixel atual
            Pixel& p = buffer[y][x];
            // define a cor do pixel no canvas
            CV::color(p.r, p.g, p.b);
            // desenha o pixel na posição com offset aplicado
            CV::point(offsetX + x, offsetY + y);
        }
    }
}

// verifica se as coordenadas estão dentro dos limites do buffer
bool Framebuffer::dentroDoBuffer(int x, int y) const
{
    return x >= 0 && x < largura && y >= 0 && y < altura;
}
