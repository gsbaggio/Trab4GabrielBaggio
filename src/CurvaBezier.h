// arquivo cabeçalho para o curvaBezier

#ifndef __CURVA_BEZIER_H__
#define __CURVA_BEZIER_H__

#include "Vector2.h"
#include <vector>

class CurvaBezier
{
private:
    std::vector<Vector2> pontosControle;    // pontos que definem a forma da curva
    std::vector<Vector2> pontosCurva;       // pontos calculados da curva final
    int numPontosCurva;                     // quantidade de pontos para discretizar a curva

    // calcula coeficiente binomial
    int binomial(int n, int k);
    
    // calcula ponto na curva de Bézier usando a fórmula de Bernstein
    Vector2 calcularPontoBezier(float t);

public:
    // construtor padrão
    CurvaBezier();
    
    // gerenciamento dos pontos de controle
    void adicionarPontoControle(Vector2 ponto);
    void removerPontoControle(int index);
    void moverPontoControle(int index, Vector2 novaPosicao);
    void limparPontosControle();
    
    // configuração da curva
    void definirNumeropontos(int numPontos);
    
    // geração da curva
    void gerarCurva();
    
    // getters para acesso aos dados
    std::vector<Vector2>& getPontosControle() { return pontosControle; }
    std::vector<Vector2>& getPontosCurva() { return pontosCurva; }
    int getNumPontosControle() { return pontosControle.size(); }
    int getNumPontosCurva() { return numPontosCurva; }
    
    // métodos de desenho
    void desenharPontosControle();
    void desenharCurva();
    void desenharPoligonControle();
    
    // verificação se um ponto está próximo de um ponto de controle
    int pontoProximoControle(Vector2 ponto, float raio = 10.0f);
};

#endif
