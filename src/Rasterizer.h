// arquivo de cabeçalho para a classe Rasterizer

#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "Vector2.h"
#include "Vector3.h"
#include "Framebuffer.h"

// estrutura que representa um vértice para rasterização
struct VerticeRaster {
    Vector2 pos2D;    // posição 2D na tela (x, y)
    Vector3 pos3D;    // posição 3D original no mundo
    Vector3 normal;   // vetor normal do vértice
    
    VerticeRaster() {}
    VerticeRaster(Vector2 _pos2D, Vector3 _pos3D, Vector3 _normal) 
        : pos2D(_pos2D), pos3D(_pos3D), normal(_normal) {}
};

// estrutura que representa uma linha horizontal (scanline) para rasterização
struct Scanline {
    int y;                              // coordenada y da scanline
    float xInicio, xFim;                // coordenadas x inicial e final
    float zInicio, zFim;                // valores z inicial e final (para z-buffer)
    Vector3 normalInicio, normalFim;    // normais inicial e final (para interpolação)
    
    Scanline(int _y, float _xInicio, float _xFim, float _zInicio, float _zFim, 
             Vector3 _normalInicio, Vector3 _normalFim)
        : y(_y), xInicio(_xInicio), xFim(_xFim), zInicio(_zInicio), zFim(_zFim),
          normalInicio(_normalInicio), normalFim(_normalFim) {}
};

class Rasterizer
{
private:
    // parâmetros de iluminação
    Vector3 luzDirecao;    // direção da luz direcional
    Vector3 corLuz;        // cor/intensidade da luz
    Vector3 corAmbiente;   // cor da luz ambiente
    Vector3 corMaterial;   // cor base do material
    
    // interpolar atributos entre dois pontos usando parâmetro t (0.0 a 1.0)
    VerticeRaster interpolar(const VerticeRaster& v1, const VerticeRaster& v2, float t);
    
    // calcular cor final usando modelo de iluminação por pixel
    Vector3 calcularIluminacao(const Vector3& normal, const Vector3& posicao3D);
    
    // desenhar uma scanline horizontal no framebuffer com interpolação de atributos
    void desenharScanline(Framebuffer& framebuffer, const Scanline& scanline);
    
public:
    // construtor padrão - inicializa parâmetros de iluminação
    Rasterizer();
    
    // métodos para configurar parâmetros de iluminação
    void definirLuz(Vector3 direcao, Vector3 cor);        // definir luz direcional
    void definirCorAmbiente(Vector3 cor);                 // definir iluminação ambiente
    void definirCorMaterial(Vector3 cor);                 // definir cor do material
    
    // rasterizar triângulo completo com z-buffer e iluminação por pixel
    void rasterizarTriangulo(Framebuffer& framebuffer, 
                           const VerticeRaster& v1, 
                           const VerticeRaster& v2, 
                           const VerticeRaster& v3);
};

#endif
