/*
implementação da classe CurvaBezier para criação e manipulação de curvas de Bézier.
permite adicionar, remover e mover pontos de controle, além de gerar e desenhar as curvas.
calcula pontos de amostragem da curva baseados nos pontos de controle.
*/

#include "CurvaBezier.h"
#include "gl_canvas2d.h"
#include <math.h>

CurvaBezier::CurvaBezier()
{
    numPontosCurva = 20; // ajustado para 20 pontos iniciais na curva
}

// calcula coeficiente binomial para o algoritmo de Bernstein
int CurvaBezier::binomial(int n, int k)
{
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    
    int result = 1;
    // calcula C(n,k) de forma iterativa
    for (int i = 1; i <= k; i++) {
        result = result * (n - i + 1) / i;
    }
    return result;
}

// calcula um ponto específico na curva de Bézier usando parâmetro t (0.0 a 1.0)
Vector2 CurvaBezier::calcularPontoBezier(float t)
{
    Vector2 ponto(0, 0);
    int n = pontosControle.size() - 1; // grau da curva
    
    // aplica a fórmula de Bernstein para cada ponto de controle
    for (int i = 0; i <= n; i++) {
        float bernstein = binomial(n, i) * pow(1 - t, n - i) * pow(t, i);
        ponto.x += bernstein * pontosControle[i].x;
        ponto.y += bernstein * pontosControle[i].y;
    }
    
    return ponto;
}

// adiciona um novo ponto de controle e regenera a curva
void CurvaBezier::adicionarPontoControle(Vector2 ponto)
{
    pontosControle.push_back(ponto);
    gerarCurva();
    
}

// remove um ponto de controle pelo índice e regenera a curva
void CurvaBezier::removerPontoControle(int index)
{
    if (index >= 0 && static_cast<size_t>(index) < pontosControle.size()) {
        pontosControle.erase(pontosControle.begin() + index);
        gerarCurva();
        
    }
}

// move um ponto de controle para nova posição e regenera a curva
void CurvaBezier::moverPontoControle(int index, Vector2 novaPosicao)
{
    if (index >= 0 && static_cast<size_t>(index) < pontosControle.size()) {
        pontosControle[index] = novaPosicao;
            gerarCurva();
        
    }
}

// limpa todos os pontos de controle e pontos da curva
void CurvaBezier::limparPontosControle()
{
    pontosControle.clear();
    pontosCurva.clear();
}

// define o número de pontos usados para discretizar a curva
void CurvaBezier::definirNumeropontos(int numPontos)
{
    numPontosCurva = numPontos;
    gerarCurva();
    
}

// gera os pontos discretos da curva baseados nos pontos de controle
void CurvaBezier::gerarCurva()
{
    pontosCurva.clear();
    
    // calcula pontos da curva variando t de 0 a 1
    for (int i = 0; i < numPontosCurva; i++) {
        float t = (float)i / (float)(numPontosCurva - 1);
        Vector2 ponto = calcularPontoBezier(t);
        pontosCurva.push_back(ponto);
    }
}

// desenha os pontos de controle como círculos vermelhos
void CurvaBezier::desenharPontosControle()
{
    CV::color(1, 0, 0); // vermelho para pontos de controle
    for (size_t i = 0; i < pontosControle.size(); i++) {
        CV::circleFill(pontosControle[i].x, pontosControle[i].y, 5, 10);
    }
}

// desenha a curva de Bézier conectando os pontos calculados
void CurvaBezier::desenharCurva()
{
    
    CV::color(0, 1, 0); // verde para a curva
    // conecta pontos consecutivos da curva com linhas
    for (size_t i = 0; i < pontosCurva.size() - 1; i++) {
        CV::line(pontosCurva[i].x, pontosCurva[i].y, 
                 pontosCurva[i+1].x, pontosCurva[i+1].y);
    }
}

// desenha o polígono de controle conectando os pontos de controle
void CurvaBezier::desenharPoligonControle()
{
    
    CV::color(0.5, 0.5, 0.5); // cinza para polígono de controle
    // conecta pontos de controle consecutivos
    for (size_t i = 0; i < pontosControle.size() - 1; i++) {
        CV::line(pontosControle[i].x, pontosControle[i].y, 
                 pontosControle[i+1].x, pontosControle[i+1].y);
    }
}

// verifica se um ponto está próximo de algum ponto de controle dentro do raio
int CurvaBezier::pontoProximoControle(Vector2 ponto, float raio)
{
    for (size_t i = 0; i < pontosControle.size(); i++) {
        Vector2 diff = ponto - pontosControle[i];
        float distancia = sqrt(diff.x * diff.x + diff.y * diff.y); // distância euclidiana
        if (distancia <= raio) {
            return static_cast<int>(i); // retorna índice do ponto encontrado
        }
    }
    return -1; // nenhum ponto encontrado
}
