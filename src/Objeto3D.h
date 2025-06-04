#ifndef __OBJETO_3D_H__
#define __OBJETO_3D_H__

#include "Vector3.h"
#include "Vector2.h"
#include "CurvaBezier.h"
#include <vector>

struct Triangulo {
    Vector3 v1, v2, v3;
    Vector3 normal;
    
    Triangulo(Vector3 _v1, Vector3 _v2, Vector3 _v3) {
        v1 = _v1;
        v2 = _v2;
        v3 = _v3;
        calcularNormal();
    }
    
    void calcularNormal() {
        Vector3 edge1 = v2 - v1;
        Vector3 edge2 = v3 - v1;
        normal = edge1.cross(edge2);
        normal.normalize();
    }
};

class Objeto3D
{
private:
    std::vector<Vector3> vertices;
    std::vector<Triangulo> triangulos;
    CurvaBezier* curvaBezier;
    
    int numDivisoesRotacao;
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
    
    // Transformações 3D para 2D
    Vector2 projetarPonto(Vector3 ponto);
    Vector3 transformarPonto(Vector3 ponto);
    
public:
    Objeto3D(CurvaBezier* curva);
    
    // Configurações
    void definirDivisoesRotacao(int divisoes);
    void mostrarVetoresNormais(bool mostrar);
    void definirModoWireframe(bool wireframe);
    void definirProjecaoPerspectiva(bool perspectiva);
    
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
    void desenharNormais();
      // Getters
    int getNumTriangulos() { return triangulos.size(); }
    int getNumVertices() { return vertices.size(); }
    bool getMostrarNormais() { return mostrarNormais; }
    bool getModoWireframe() { return modoWireframe; }
    bool getProjecaoPerspectiva() { return projecaoPerspectiva; }
    float getEscala() { return escala; }
};

#endif
