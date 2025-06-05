#include "Objeto3D.h"
#include "gl_canvas2d.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265359
#endif

Objeto3D::Objeto3D(CurvaBezier* curva)
{
    curvaBezier = curva;
    numDivisoesRotacao = 10; // Ajustado para 10 divisões iniciais
    sweepTranslacional = 0.0f; // Sem incremento vertical inicial
    mostrarNormais = false;
    modoWireframe = true;
    projecaoPerspectiva = false; // Já está em ortográfica
    distanciaCamera = 800.0f; // Aumentado de 300 para 800 - câmera mais distante
    
    // Inicializar transformações
    rotacaoX = rotacaoY = rotacaoZ = 0.0f;
    translacaoX = translacaoY = translacaoZ = 0.0f;
    escala = 0.3f; // Reduzido de 1.0 para 0.3 - objeto menor inicialmente
}

void Objeto3D::definirDivisoesRotacao(int divisoes)
{
    numDivisoesRotacao = divisoes;
    atualizarObjeto();
}

void Objeto3D::definirSweepTranslacional(float incremento)
{
    sweepTranslacional = incremento;
    atualizarObjeto();
}

void Objeto3D::mostrarVetoresNormais(bool mostrar)
{
    mostrarNormais = mostrar;
}

void Objeto3D::definirModoWireframe(bool wireframe)
{
    modoWireframe = wireframe;
}

void Objeto3D::definirProjecaoPerspectiva(bool perspectiva)
{
    projecaoPerspectiva = perspectiva;
}

void Objeto3D::rotacionar(float deltaX, float deltaY, float deltaZ)
{
    rotacaoX += deltaX;
    rotacaoY += deltaY;
    rotacaoZ += deltaZ;
}

void Objeto3D::transladar(float deltaX, float deltaY, float deltaZ)
{
    translacaoX += deltaX;
    translacaoY += deltaY;
    translacaoZ += deltaZ;
}

void Objeto3D::definirEscala(float novaEscala)
{
    escala = novaEscala;
}

void Objeto3D::resetarTransformacoes()
{
    rotacaoX = rotacaoY = rotacaoZ = 0.0f;
    translacaoX = translacaoY = translacaoZ = 0.0f;
    escala = 0.3f; // Mesma escala inicial
    distanciaCamera = 800.0f; // Mesma distância inicial
}

void Objeto3D::definirDistanciaCamera(float distancia)
{
    distanciaCamera = distancia;
}

Vector3 Objeto3D::transformarPonto(Vector3 ponto)
{
    Vector3 p = ponto;
    
    // Aplicar escala
    p = p * escala;
    
    // Aplicar rotações
    // Rotação em X
    if (rotacaoX != 0) {
        float cosX = cos(rotacaoX);
        float sinX = sin(rotacaoX);
        float y = p.y * cosX - p.z * sinX;
        float z = p.y * sinX + p.z * cosX;
        p.y = y;
        p.z = z;
    }
    
    // Rotação em Y
    if (rotacaoY != 0) {
        float cosY = cos(rotacaoY);
        float sinY = sin(rotacaoY);
        float x = p.x * cosY + p.z * sinY;
        float z = -p.x * sinY + p.z * cosY;
        p.x = x;
        p.z = z;
    }
    
    // Rotação em Z
    if (rotacaoZ != 0) {
        float cosZ = cos(rotacaoZ);
        float sinZ = sin(rotacaoZ);
        float x = p.x * cosZ - p.y * sinZ;
        float y = p.x * sinZ + p.y * cosZ;
        p.x = x;
        p.y = y;
    }
    
    // Aplicar translação
    p.x += translacaoX;
    p.y += translacaoY;
    p.z += translacaoZ;
    
    return p;
}

Vector2 Objeto3D::projetarPonto(Vector3 ponto)
{
    Vector3 p = transformarPonto(ponto);
    Vector2 resultado;
    
    if (projecaoPerspectiva) {
        // Projeção perspectiva
        float fator = distanciaCamera / (distanciaCamera + p.z);
        resultado.x = p.x * fator;
        resultado.y = p.y * fator;
    } else {
        // Projeção ortográfica
        resultado.x = p.x;
        resultado.y = p.y;
    }
    
    return resultado;
}

void Objeto3D::gerarSweepRotacional()
{
    vertices.clear();
    triangulos.clear();
    
    std::vector<Vector2>& pontosCurva = curvaBezier->getPontosCurva();
    if (pontosCurva.size() < 2) return;
      // Gerar vértices através de rotação
    float anguloIncremento = 2.0f * PI / numDivisoesRotacao;
      for (int i = 0; i <= numDivisoesRotacao; i++) {
        float angulo = i * anguloIncremento;
        float cosA = cos(angulo);
        float sinA = sin(angulo);
          // Calcular deslocamento vertical para sweep translacional
        // Valores positivos fazem crescer para cima, negativos para baixo
        float deslocamentoY = -i * sweepTranslacional;
        
        for (int j = 0; j < pontosCurva.size(); j++) {
            Vector2& p = pontosCurva[j];
            float raio = fabs(p.x); // Usar fabs ao invés de abs para float
            
            Vector3 vertice;
            vertice.x = raio * cosA;
            vertice.y = p.y + deslocamentoY; // Adicionar incremento vertical
            vertice.z = raio * sinA;
            
            vertices.push_back(vertice);
        }
    }
    
    // Gerar triângulos
    int numPontosCurva = pontosCurva.size();
    for (int i = 0; i < numDivisoesRotacao; i++) {
        for (int j = 0; j < numPontosCurva - 1; j++) {
            int idx1 = i * numPontosCurva + j;
            int idx2 = i * numPontosCurva + j + 1;
            int idx3 = ((i + 1) % (numDivisoesRotacao + 1)) * numPontosCurva + j;
            int idx4 = ((i + 1) % (numDivisoesRotacao + 1)) * numPontosCurva + j + 1;
            
            // Criar dois triângulos para formar um quad
            if (idx1 < vertices.size() && idx2 < vertices.size() && idx3 < vertices.size()) {
                triangulos.push_back(Triangulo(vertices[idx1], vertices[idx2], vertices[idx3]));
            }
            if (idx2 < vertices.size() && idx3 < vertices.size() && idx4 < vertices.size()) {
                triangulos.push_back(Triangulo(vertices[idx2], vertices[idx4], vertices[idx3]));
            }
        }
    }
}

void Objeto3D::atualizarObjeto()
{
    gerarSweepRotacional();
}

void Objeto3D::desenhar()
{
    if (triangulos.empty()) return;
    
    CV::color(0, 0, 1); // Azul para wireframe
    
    for (int i = 0; i < triangulos.size(); i++) {
        Triangulo& tri = triangulos[i];
        
        Vector2 p1 = projetarPonto(tri.v1);
        Vector2 p2 = projetarPonto(tri.v2);
        Vector2 p3 = projetarPonto(tri.v3);
        
        // Desenhar triângulo em wireframe
        CV::line(p1.x, p1.y, p2.x, p2.y);
        CV::line(p2.x, p2.y, p3.x, p3.y);
        CV::line(p3.x, p3.y, p1.x, p1.y);
    }
}

void Objeto3D::desenharNormais()
{
    if (!mostrarNormais || triangulos.empty()) return;
    
    CV::color(1, 1, 0); // Amarelo para normais
    
    for (int i = 0; i < triangulos.size(); i++) {
        Triangulo& tri = triangulos[i];
        
        // Calcular centro do triângulo
        Vector3 centro;
        centro.x = (tri.v1.x + tri.v2.x + tri.v3.x) / 3.0f;
        centro.y = (tri.v1.y + tri.v2.y + tri.v3.y) / 3.0f;
        centro.z = (tri.v1.z + tri.v2.z + tri.v3.z) / 3.0f;
        
        // Ponto final da normal
        Vector3 fimNormal;
        fimNormal.x = centro.x + tri.normal.x * 20.0f;
        fimNormal.y = centro.y + tri.normal.y * 20.0f;
        fimNormal.z = centro.z + tri.normal.z * 20.0f;
        
        Vector2 p1 = projetarPonto(centro);
        Vector2 p2 = projetarPonto(fimNormal);
        
        CV::line(p1.x, p1.y, p2.x, p2.y);
    }
}
