#ifndef __CURVA_BEZIER_H__
#define __CURVA_BEZIER_H__

#include "Vector2.h"
#include <vector>

class CurvaBezier
{
private:
    std::vector<Vector2> pontosControle;
    std::vector<Vector2> pontosCurva;
    int numPontosCurva;

    // Calcula coeficiente binomial
    int binomial(int n, int k);
    
    // Calcula ponto na curva de Bezier usando a fórmula de Bernstein
    Vector2 calcularPontoBezier(float t);

public:
    CurvaBezier();
    
    // Gerenciamento dos pontos de controle
    void adicionarPontoControle(Vector2 ponto);
    void removerPontoControle(int index);
    void moverPontoControle(int index, Vector2 novaPosicao);
    void limparPontosControle();
    
    // Configuração da curva
    void definirNumeropontos(int numPontos);
    
    // Geração da curva
    void gerarCurva();
    
    // Getters
    std::vector<Vector2>& getPontosControle() { return pontosControle; }
    std::vector<Vector2>& getPontosCurva() { return pontosCurva; }
    int getNumPontosControle() { return pontosControle.size(); }
    int getNumPontosCurva() { return numPontosCurva; }
    
    // Desenho
    void desenharPontosControle();
    void desenharCurva();
    void desenharPoligonControle();
    
    // Verificação se um ponto está próximo de um ponto de controle
    int pontoProximoControle(Vector2 ponto, float raio = 10.0f);
};

#endif
