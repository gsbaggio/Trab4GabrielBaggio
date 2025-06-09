// arquivo cabeçalho para a aplicação 3D

#ifndef __APLICACAO_3D_H__
#define __APLICACAO_3D_H__

#include "CurvaBezier.h"
#include "Objeto3D.h"
#include "Vector2.h"

class Aplicacao3D
{
private:
    CurvaBezier* curva;
    Objeto3D* objeto;
    
    // controles de interface
    bool arrastandoPonto;
    int pontoSendoArrastado;
      // controles de visualização 3D
    bool rotacionandoObjeto;
    Vector2 ultimaPosicaoMouse;
    Vector2 posicaoAtualMouse; // para armazenar posição atual do mouse
    
    // dimensões da tela
    int larguraTela, alturaTela;
    int metadeLargura; // divisão da tela
    
    // interface gráfica
    void desenharEdicaoCurva();
    void desenharVisualizacao3D();
    void desenharInterface();
    
    // verificar em qual metade da tela o mouse está
    bool mouseNaEdicao(int x);
    bool mouseNaVisualizacao(int x);
    
public:
    Aplicacao3D();
    ~Aplicacao3D();
    
    void inicializar();
    void atualizar();
    void desenhar();
    void definirDimensoesTela(int largura, int altura);
    
    // eventos
    void onMouseClick(int button, int state, int x, int y);
    void onMouseMove(int x, int y);
    void onKeyboard(int key);
};

#endif
