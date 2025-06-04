#include "CurvaBezier.h"
#include "gl_canvas2d.h"
#include <math.h>

CurvaBezier::CurvaBezier()
{
    numPontosCurva = 100; // Aumentado para 100 pontos
}

int CurvaBezier::binomial(int n, int k)
{
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    
    int result = 1;
    for (int i = 1; i <= k; i++) {
        result = result * (n - i + 1) / i;
    }
    return result;
}

Vector2 CurvaBezier::calcularPontoBezier(float t)
{
    Vector2 ponto(0, 0);
    int n = pontosControle.size() - 1;
    
    for (int i = 0; i <= n; i++) {
        float bernstein = binomial(n, i) * pow(1 - t, n - i) * pow(t, i);
        ponto.x += bernstein * pontosControle[i].x;
        ponto.y += bernstein * pontosControle[i].y;
    }
    
    return ponto;
}

void CurvaBezier::adicionarPontoControle(Vector2 ponto)
{
    pontosControle.push_back(ponto);
    if (pontosControle.size() >= 2) {
        gerarCurva();
    }
}

void CurvaBezier::removerPontoControle(int index)
{
    if (index >= 0 && index < pontosControle.size()) {
        pontosControle.erase(pontosControle.begin() + index);
        if (pontosControle.size() >= 2) {
            gerarCurva();
        }
    }
}

void CurvaBezier::moverPontoControle(int index, Vector2 novaPosicao)
{
    if (index >= 0 && index < pontosControle.size()) {
        pontosControle[index] = novaPosicao;
        if (pontosControle.size() >= 2) {
            gerarCurva();
        }
    }
}

void CurvaBezier::limparPontosControle()
{
    pontosControle.clear();
    pontosCurva.clear();
}

void CurvaBezier::definirNumeropontos(int numPontos)
{
    numPontosCurva = numPontos;
    if (pontosControle.size() >= 2) {
        gerarCurva();
    }
}

void CurvaBezier::gerarCurva()
{
    pontosCurva.clear();
    
    if (pontosControle.size() < 2) return;
    
    for (int i = 0; i <= numPontosCurva; i++) {
        float t = (float)i / (float)numPontosCurva;
        Vector2 ponto = calcularPontoBezier(t);
        pontosCurva.push_back(ponto);
    }
}

void CurvaBezier::desenharPontosControle()
{
    CV::color(1, 0, 0); // Vermelho para pontos de controle
    for (int i = 0; i < pontosControle.size(); i++) {
        CV::circleFill(pontosControle[i].x, pontosControle[i].y, 5, 10);
    }
}

void CurvaBezier::desenharCurva()
{
    if (pontosCurva.size() < 2) return;
    
    CV::color(0, 1, 0); // Verde para a curva
    for (int i = 0; i < pontosCurva.size() - 1; i++) {
        CV::line(pontosCurva[i].x, pontosCurva[i].y, 
                 pontosCurva[i+1].x, pontosCurva[i+1].y);
    }
}

void CurvaBezier::desenharPoligonControle()
{
    if (pontosControle.size() < 2) return;
    
    CV::color(0.5, 0.5, 0.5); // Cinza para polígono de controle
    for (int i = 0; i < pontosControle.size() - 1; i++) {
        CV::line(pontosControle[i].x, pontosControle[i].y, 
                 pontosControle[i+1].x, pontosControle[i+1].y);
    }
}

int CurvaBezier::pontoProximoControle(Vector2 ponto, float raio)
{
    for (int i = 0; i < pontosControle.size(); i++) {
        Vector2 diff = ponto - pontosControle[i];
        float distancia = sqrt(diff.x * diff.x + diff.y * diff.y);
        if (distancia <= raio) {
            return i;
        }
    }
    return -1;
}
