#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include "Vector2.h"
#include "Vector3.h"
#include "Framebuffer.h"

struct VerticeRaster {
    Vector2 pos2D;
    Vector3 pos3D;
    Vector3 normal;
    
    VerticeRaster() {}
    VerticeRaster(Vector2 _pos2D, Vector3 _pos3D, Vector3 _normal) 
        : pos2D(_pos2D), pos3D(_pos3D), normal(_normal) {}
};

struct Scanline {
    int y;
    float xInicio, xFim;
    float zInicio, zFim;
    Vector3 normalInicio, normalFim;
    
    Scanline(int _y, float _xInicio, float _xFim, float _zInicio, float _zFim, 
             Vector3 _normalInicio, Vector3 _normalFim)
        : y(_y), xInicio(_xInicio), xFim(_xFim), zInicio(_zInicio), zFim(_zFim),
          normalInicio(_normalInicio), normalFim(_normalFim) {}
};

class Rasterizer
{
private:
    // Iluminação
    Vector3 luzDirecao;
    Vector3 corLuz;
    Vector3 corAmbiente;
    Vector3 corMaterial;
    
    // Interpolar atributos entre dois pontos
    VerticeRaster interpolar(const VerticeRaster& v1, const VerticeRaster& v2, float t);
    
    // Calcular cor com iluminação por pixel
    Vector3 calcularIluminacao(const Vector3& normal, const Vector3& posicao3D);
    
    // Desenhar scanline
    void desenharScanline(Framebuffer& framebuffer, const Scanline& scanline);
    
public:
    Rasterizer();
    
    // Configurar iluminação
    void definirLuz(Vector3 direcao, Vector3 cor);
    void definirCorAmbiente(Vector3 cor);
    void definirCorMaterial(Vector3 cor);
    
    // Rasterizar triângulo com z-buffer e iluminação por pixel
    void rasterizarTriangulo(Framebuffer& framebuffer, 
                           const VerticeRaster& v1, 
                           const VerticeRaster& v2, 
                           const VerticeRaster& v3);
};

#endif
