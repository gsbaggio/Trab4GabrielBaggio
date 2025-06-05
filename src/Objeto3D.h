#ifndef __OBJETO_3D_H__
#define __OBJETO_3D_H__

#include "Vector3.h"
#include "Vector2.h"
#include "CurvaBezier.h"
#include "Framebuffer.h"
#include "Rasterizer.h"
#include <vector>

struct Triangulo {
    Vector3 v1, v2, v3;
    Vector3 normal;
    Vector3 n1, n2, n3; // Per-vertex normals for smooth shading
    
    Triangulo(Vector3 _v1, Vector3 _v2, Vector3 _v3) {
        v1 = _v1;
        v2 = _v2;
        v3 = _v3;
        calcularNormal();
        // Initialize vertex normals to face normal
        n1 = n2 = n3 = normal;
    }
    
    void calcularNormal() {
        Vector3 edge1 = v2 - v1;
        Vector3 edge2 = v3 - v1;
        normal = edge1.cross(edge2);
        normal.normalize();
    }
    
    void definirNormaisVertices(Vector3 _n1, Vector3 _n2, Vector3 _n3) {
        n1 = _n1; n2 = _n2; n3 = _n3;
    }
};

class Objeto3D
{
private:
    std::vector<Vector3> vertices;
    std::vector<Triangulo> triangulos;
    CurvaBezier* curvaBezier;
    
    // Rendering system
    Framebuffer* framebuffer;
    Rasterizer* rasterizer;
    
    int numDivisoesRotacao;
    float sweepTranslacional; // Incremento Y por divisão
    bool mostrarNormais;
    bool modoWireframe;
    
    // Variáveis de transformação
    float rotacaoX, rotacaoY, rotacaoZ;
    float translacaoX, translacaoY, translacaoZ;
    float escala;
    
    // Modo de visualização
    bool projecaoPerspectiva;
    float distanciaCamera;
    
    // Gerar objeto 3D através de sweep rotacional
    void gerarSweepRotacional();
    
    // Calculate smooth vertex normals
    void calcularNormaisVertices();
    
    // Transformações 3D para 2D
    Vector2 projetarPonto(Vector3 ponto);
    Vector2 projetarPontoFramebuffer(Vector3 ponto);
    Vector3 transformarPonto(Vector3 ponto);
    Vector3 transformarNormal(Vector3 normal);
    
public:
    Objeto3D(CurvaBezier* curva);
    ~Objeto3D();
    
    // Configurações
    void definirDivisoesRotacao(int divisoes);
    void mostrarVetoresNormais(bool mostrar);
    void definirModoWireframe(bool wireframe);
    void definirProjecaoPerspectiva(bool perspectiva);
    
    // Rendering configuration
    void inicializarFramebuffer(int largura, int altura);
    void definirLuz(Vector3 direcao, Vector3 cor);
    void definirCorAmbiente(Vector3 cor);
    void definirCorMaterial(Vector3 cor);
    
    // Transformações
    void rotacionar(float deltaX, float deltaY, float deltaZ);
    void transladar(float deltaX, float deltaY, float deltaZ);
    void definirEscala(float novaEscala);
    void resetarTransformacoes();
    
    // Camera
    void definirDistanciaCamera(float distancia);
    
    // Geração e renderização
    void atualizarObjeto();
    void desenhar();
    void desenharNormais();    // Getters
    int getNumTriangulos() { return triangulos.size(); }
    int getNumVertices() { return vertices.size(); }
    int getNumDivisoesRotacao() { return numDivisoesRotacao; }
    float getSweepTranslacional() { return sweepTranslacional; }
    bool getMostrarNormais() { return mostrarNormais; }
    bool getModoWireframe() { return modoWireframe; }
    bool getProjecaoPerspectiva() { return projecaoPerspectiva; }
    float getEscala() { return escala; }
    Framebuffer* getFramebuffer() { return framebuffer; }
    
    // Setters para sweep translacional
    void definirSweepTranslacional(float incremento);
};

#endif
