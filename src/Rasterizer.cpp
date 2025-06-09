#include "Rasterizer.h"
#include <algorithm>
#include <cmath>

Rasterizer::Rasterizer()
{
    // Configurações padrão de iluminação
    luzDirecao = Vector3(0.0f, 0.0f, 1.0f); // Luz vindo de frente
    luzDirecao.normalize();
    
    corLuz = Vector3(1.0f, 1.0f, 1.0f);     // Luz branca
    corAmbiente = Vector3(0.2f, 0.2f, 0.2f); // Ambiente escuro
    corMaterial = Vector3(0.8f, 0.8f, 0.8f); // Material cinza claro
}

void Rasterizer::definirLuz(Vector3 direcao, Vector3 cor)
{
    luzDirecao = direcao;
    luzDirecao.normalize();
    corLuz = cor;
}

void Rasterizer::definirCorAmbiente(Vector3 cor)
{
    corAmbiente = cor;
}

void Rasterizer::definirCorMaterial(Vector3 cor)
{
    corMaterial = cor;
}

VerticeRaster Rasterizer::interpolar(const VerticeRaster& v1, const VerticeRaster& v2, float t)
{
    VerticeRaster resultado;
    
    // Interpolar posição 2D
    resultado.pos2D.x = v1.pos2D.x + t * (v2.pos2D.x - v1.pos2D.x);
    resultado.pos2D.y = v1.pos2D.y + t * (v2.pos2D.y - v1.pos2D.y);
    
    // Interpolar posição 3D
    resultado.pos3D.x = v1.pos3D.x + t * (v2.pos3D.x - v1.pos3D.x);
    resultado.pos3D.y = v1.pos3D.y + t * (v2.pos3D.y - v1.pos3D.y);
    resultado.pos3D.z = v1.pos3D.z + t * (v2.pos3D.z - v1.pos3D.z);
    
    // Interpolar normal
    resultado.normal.x = v1.normal.x + t * (v2.normal.x - v1.normal.x);
    resultado.normal.y = v1.normal.y + t * (v2.normal.y - v1.normal.y);
    resultado.normal.z = v1.normal.z + t * (v2.normal.z - v1.normal.z);
    resultado.normal.normalize();
    
    return resultado;
}

Vector3 Rasterizer::calcularIluminacao(const Vector3& normal, const Vector3& posicao3D)
{
    // Calcular intensidade da luz difusa
    float intensidadeDifusa = std::max(0.0f, normal.dot(luzDirecao));
    
    // Combinar luz ambiente e difusa
    Vector3 corFinal;
    corFinal.x = corMaterial.x * (corAmbiente.x + corLuz.x * intensidadeDifusa);
    corFinal.y = corMaterial.y * (corAmbiente.y + corLuz.y * intensidadeDifusa);
    corFinal.z = corMaterial.z * (corAmbiente.z + corLuz.z * intensidadeDifusa);
    
    // Garantir que as cores estejam no range [0,1]
    corFinal.x = std::min(1.0f, std::max(0.0f, corFinal.x));
    corFinal.y = std::min(1.0f, std::max(0.0f, corFinal.y));
    corFinal.z = std::min(1.0f, std::max(0.0f, corFinal.z));
    
    return corFinal;
}

void Rasterizer::desenharScanline(Framebuffer& framebuffer, const Scanline& scanline)
{
    // Usar coordenadas de ponto flutuante para melhor precisão
    int xInicio = (int)std::floor(scanline.xInicio + 0.5f); // Arredondar para o pixel mais próximo
    int xFim = (int)std::floor(scanline.xFim + 0.5f);       // Arredondar para o pixel mais próximo
    
    // Garantir que pelo menos um pixel seja desenhado se a scanline tem largura
    if (xFim < xInicio && scanline.xFim > scanline.xInicio) {
        xFim = xInicio;
    }
    
    if (xFim < xInicio) return;
    
    float deltaX = scanline.xFim - scanline.xInicio;
    if (deltaX == 0.0f) {
        // Caso especial: linha vertical (um único pixel)
        deltaX = 1.0f;
    }
    
    for (int x = xInicio; x <= xFim; x++) {
        if (x < 0 || x >= framebuffer.getLargura()) continue;
        
        // Calcular fator de interpolação
        float t = (deltaX != 0.0f) ? (x - scanline.xInicio) / deltaX : 0.0f;
        t = std::max(0.0f, std::min(1.0f, t)); // Clampar t entre 0 e 1
        
        // Interpolar profundidade
        float z = scanline.zInicio + t * (scanline.zFim - scanline.zInicio);
        
        // Interpolar normal
        Vector3 normal;
        normal.x = scanline.normalInicio.x + t * (scanline.normalFim.x - scanline.normalInicio.x);
        normal.y = scanline.normalInicio.y + t * (scanline.normalFim.y - scanline.normalInicio.y);
        normal.z = scanline.normalInicio.z + t * (scanline.normalFim.z - scanline.normalInicio.z);
        normal.normalize();
        
        // Calcular posição 3D aproximada para este pixel
        Vector3 posicao3D(x, scanline.y, z);
        
        // Calcular iluminação para este pixel
        Vector3 cor = calcularIluminacao(normal, posicao3D);
        
        // Desenhar pixel no framebuffer com teste de profundidade
        framebuffer.setPixel(x, scanline.y, cor.x, cor.y, cor.z, z);
    }
}

void Rasterizer::rasterizarTriangulo(Framebuffer& framebuffer, 
                                   const VerticeRaster& v1, 
                                   const VerticeRaster& v2, 
                                   const VerticeRaster& v3)
{
    // Ordenar vértices por Y (v1 = topo, v3 = base)
    std::vector<VerticeRaster> vertices = {v1, v2, v3};
    std::sort(vertices.begin(), vertices.end(), 
              [](const VerticeRaster& a, const VerticeRaster& b) {
                  return a.pos2D.y < b.pos2D.y;
              });
    
    const VerticeRaster& vTop = vertices[0];
    const VerticeRaster& vMid = vertices[1];
    const VerticeRaster& vBot = vertices[2];
      // Verificar se o triângulo tem altura suficiente para rasterizar
    if (vBot.pos2D.y - vTop.pos2D.y < 0.5f) return;
    
    // Usar arredondamento mais preciso para as bordas dos triângulos
    int yInicio = (int)std::floor(vTop.pos2D.y + 0.5f);
    int yMeio = (int)std::floor(vMid.pos2D.y + 0.5f);
    int yFim = (int)std::floor(vBot.pos2D.y + 0.5f);
    
    for (int y = yInicio; y <= yFim; y++) {
        if (y < 0 || y >= framebuffer.getAltura()) continue;
        
        // Calcular intersecções das arestas com a scanline
        VerticeRaster esquerda, direita;
          if (y <= yMeio) {
            // Parte superior: interpolar entre vTop-vMid e vTop-vBot
            float t1 = (vMid.pos2D.y - vTop.pos2D.y != 0) ? 
                      (y - vTop.pos2D.y) / (vMid.pos2D.y - vTop.pos2D.y) : 0.0f;
            float t2 = (vBot.pos2D.y - vTop.pos2D.y != 0) ? 
                      (y - vTop.pos2D.y) / (vBot.pos2D.y - vTop.pos2D.y) : 0.0f;
            
            // Clampar os fatores de interpolação para evitar extrapolação
            t1 = std::max(0.0f, std::min(1.0f, t1));
            t2 = std::max(0.0f, std::min(1.0f, t2));
            
            VerticeRaster p1 = interpolar(vTop, vMid, t1);
            VerticeRaster p2 = interpolar(vTop, vBot, t2);
            
            if (p1.pos2D.x < p2.pos2D.x) {
                esquerda = p1; direita = p2;
            } else {
                esquerda = p2; direita = p1;
            }        } else {
            // Parte inferior: interpolar entre vMid-vBot e vTop-vBot
            float t1 = (vBot.pos2D.y - vMid.pos2D.y != 0) ? 
                      (y - vMid.pos2D.y) / (vBot.pos2D.y - vMid.pos2D.y) : 0.0f;
            float t2 = (vBot.pos2D.y - vTop.pos2D.y != 0) ? 
                      (y - vTop.pos2D.y) / (vBot.pos2D.y - vTop.pos2D.y) : 0.0f;
            
            // Clampar os fatores de interpolação para evitar extrapolação
            t1 = std::max(0.0f, std::min(1.0f, t1));
            t2 = std::max(0.0f, std::min(1.0f, t2));
            
            VerticeRaster p1 = interpolar(vMid, vBot, t1);
            VerticeRaster p2 = interpolar(vTop, vBot, t2);
            
            if (p1.pos2D.x < p2.pos2D.x) {
                esquerda = p1; direita = p2;
            } else {
                esquerda = p2; direita = p1;
            }
        }
        
        // Criar e desenhar scanline
        Scanline scanline(y, esquerda.pos2D.x, direita.pos2D.x, 
                         esquerda.pos3D.z, direita.pos3D.z,
                         esquerda.normal, direita.normal);
        
        desenharScanline(framebuffer, scanline);
    }
}
