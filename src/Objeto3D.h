/*
Aruivo de cabeçalho para a classe Objeto3D
*/

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
    Vector3 n1, n2, n3; // normais por vértice para sombreamento suave
    
    Triangulo(Vector3 _v1, Vector3 _v2, Vector3 _v3) {
        v1 = _v1;
        v2 = _v2;
        v3 = _v3;
        calcularNormal();
        // inicializar normais dos vértices com a normal da face
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
    
    // sistema de renderização
    Framebuffer* framebuffer;
    Rasterizer* rasterizer;
    
    int numDivisoesRotacao;
    float sweepTranslacional; // incremento Y por divisão
    bool mostrarNormais;
    bool modoWireframe;
    
    // variáveis de transformação
    float rotacaoX, rotacaoY, rotacaoZ;
    float translacaoX, translacaoY, translacaoZ;
    float escala;
    
    // modo de visualização
    bool projecaoPerspectiva;
    float distanciaCamera;
    
    // gerar objeto 3D através de sweep rotacional
    void gerarSweepRotacional();
    
    // calcular normais suaves dos vértices
    void calcularNormaisVertices();
    
    // transformações 3D para 2D
    Vector2 projetarPonto(Vector3 ponto);
    Vector2 projetarPontoFramebuffer(Vector3 ponto);
    Vector3 transformarPonto(Vector3 ponto);
    Vector3 transformarNormal(Vector3 normal);
    
public:
    Objeto3D(CurvaBezier* curva);
    ~Objeto3D();
    
    // configurações
    void definirDivisoesRotacao(int divisoes);
    void mostrarVetoresNormais(bool mostrar);
    void definirModoWireframe(bool wireframe);
    void definirProjecaoPerspectiva(bool perspectiva);
    
    // configuração de renderização
    void inicializarFramebuffer(int largura, int altura);
    void definirLuz(Vector3 direcao, Vector3 cor);
    void definirCorAmbiente(Vector3 cor);
    void definirCorMaterial(Vector3 cor);
    
    // transformações
    void rotacionar(float deltaX, float deltaY, float deltaZ);
    void transladar(float deltaX, float deltaY, float deltaZ);
    void definirEscala(float novaEscala);
    void resetarTransformacoes();
    
    // camera
    void definirDistanciaCamera(float distancia);
    
    // geração e renderização
    void atualizarObjeto();
    void desenhar();
    void desenharNormais();    // getters
    int getNumTriangulos() { return triangulos.size(); }
    int getNumVertices() { return vertices.size(); }
    int getNumDivisoesRotacao() { return numDivisoesRotacao; }
    float getSweepTranslacional() { return sweepTranslacional; }
    bool getMostrarNormais() { return mostrarNormais; }
    bool getModoWireframe() { return modoWireframe; }
    bool getProjecaoPerspectiva() { return projecaoPerspectiva; }
    float getEscala() { return escala; }
    Framebuffer* getFramebuffer() { return framebuffer; }
    
    // setters para sweep translacional
    void definirSweepTranslacional(float incremento);
};

#endif
