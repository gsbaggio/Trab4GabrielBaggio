// arquivo cabeçalho para o framebuffer

#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Vector2.h"
#include "Vector3.h"
#include <vector>
#include <limits>

// estrutura que representa um pixel com cor RGB e profundidade
struct Pixel {
    float r, g, b;    // componentes de cor (vermelho, verde, azul)
    float depth;      // valor de profundidade para z-buffer
    
    // construtor padrão inicializa pixel preto com profundidade máxima
    Pixel() : r(0.0f), g(0.0f), b(0.0f), depth(std::numeric_limits<float>::max()) {}
    // construtor com parâmetros para definir cor e profundidade
    Pixel(float _r, float _g, float _b, float _d = std::numeric_limits<float>::max()) 
        : r(_r), g(_g), b(_b), depth(_d) {}
};

// classe que representa um buffer de pixels para renderização
class Framebuffer 
{
private:
    int largura, altura;                      // dimensões do buffer
    std::vector<std::vector<Pixel>> buffer;   // matriz bidimensional de pixels
    
public:
    // construtor que inicializa buffer com largura e altura especificadas
    Framebuffer(int w, int h);
    
    // limpa o buffer preenchendo com a cor especificada (preto por padrão)
    void limpar(float r = 0.0f, float g = 0.0f, float b = 0.0f);
    // define um pixel na posição (x,y) com cor e profundidade, retorna true se bem-sucedido
    bool setPixel(int x, int y, float r, float g, float b, float depth);
    // obtém o pixel na posição (x,y)
    Pixel getPixel(int x, int y) const;
    
    // desenha o conteúdo do buffer na tela com offset especificado
    void desenharNaTela(int offsetX, int offsetY);
    
    // métodos para obter as dimensões do buffer
    int getLargura() const { return largura; }
    int getAltura() const { return altura; }
    
    // verifica se as coordenadas (x,y) estão dentro dos limites do buffer
    bool dentroDoBuffer(int x, int y) const;
};

#endif
