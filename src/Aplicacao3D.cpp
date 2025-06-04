#include "Aplicacao3D.h"
#include "gl_canvas2d.h"
#include <stdio.h>

Aplicacao3D::Aplicacao3D()
{
    arrastandoPonto = false;
    pontoSendoArrastado = -1;
    rotacionandoObjeto = false;
    larguraTela = 1080;
    alturaTela = 720;
    metadeLargura = larguraTela / 2;
    
    // Criar única curva
    curva = new CurvaBezier();
    objeto = new Objeto3D(curva);
    
    // Adicionar alguns pontos de exemplo
    curva->adicionarPontoControle(Vector2(100, 200));  // Base
    curva->adicionarPontoControle(Vector2(150, 300));  // Corpo inferior
    curva->adicionarPontoControle(Vector2(120, 400));  // Corpo médio
    curva->adicionarPontoControle(Vector2(80, 500));   // Gargalo
    curva->adicionarPontoControle(Vector2(100, 600));  // Topo
}

Aplicacao3D::~Aplicacao3D()
{
    delete curva;
    delete objeto;
}

void Aplicacao3D::inicializar()
{
    // Configurações iniciais do objeto 3D
    objeto->definirProjecaoPerspectiva(true);
    objeto->definirDivisoesRotacao(20);
}

void Aplicacao3D::definirDimensoesTela(int largura, int altura)
{
    larguraTela = largura;
    alturaTela = altura;
    metadeLargura = largura / 2;
}

bool Aplicacao3D::mouseNaEdicao(int x)
{
    return x < metadeLargura;
}

bool Aplicacao3D::mouseNaVisualizacao(int x)
{
    return x >= metadeLargura;
}

void Aplicacao3D::atualizar()
{
    // Atualizar objeto 3D quando necessário
    if (curva->getNumPontosCurva() > 0) {
        objeto->atualizarObjeto();
    }
}

void Aplicacao3D::desenhar()
{
    CV::clear(0.1f, 0.1f, 0.1f);
    
    // PRIMEIRO: Desenhar a projeção 3D como fundo
    desenharVisualizacao3D();
    
    // SEGUNDO: Mascarar as bordas para limitar a área 3D
    CV::color(0.1f, 0.1f, 0.1f); // Cor de fundo para mascarar
    
    // Mascarar borda superior da área 3D
    CV::rectFill(metadeLargura, 0, larguraTela, 50);
    
    // Mascarar borda inferior da área 3D  
    CV::rectFill(metadeLargura, alturaTela - 50, larguraTela, alturaTela);
    
    // Mascarar borda esquerda da área 3D (entre as duas metades)
    CV::rectFill(metadeLargura, 0, metadeLargura + 10, alturaTela);
    
    // Mascarar borda direita da área 3D
    CV::rectFill(larguraTela - 10, 0, larguraTela, alturaTela);
    
    // TERCEIRO: Desenhar elementos da interface por cima
    // Desenhar linha divisória
    CV::color(0.5f, 0.5f, 0.5f);
    CV::line(metadeLargura, 0, metadeLargura, alturaTela);
    
    // Desenhar lado esquerdo - edição de curva
    desenharEdicaoCurva();
    
    // Desenhar instruções e interface
    desenharInterface();
}

void Aplicacao3D::desenharEdicaoCurva()
{
    // Desenhar área de trabalho da curva
    CV::color(0.2f, 0.2f, 0.2f);
    CV::rectFill(10, 50, metadeLargura - 10, alturaTela - 50);
    CV::color(0.5f, 0.5f, 0.5f);
    CV::rect(10, 50, metadeLargura - 10, alturaTela - 50);
    
    // Desenhar curva e pontos de controle
    if (curva) {
        curva->desenharPoligonControle();
        curva->desenharCurva();
        curva->desenharPontosControle();
    }
}

void Aplicacao3D::desenharVisualizacao3D()
{
    // Desenhar objeto 3D no centro da área direita (sem limitações de área ainda)
    CV::translate(metadeLargura + (metadeLargura / 2), alturaTela / 2);
    
    if (objeto && curva->getNumPontosControle() >= 2) {
        objeto->desenhar();
        if (objeto->getMostrarNormais()) {
            objeto->desenharNormais();
        }
    }
    
    CV::translate(0, 0); // Reset da translação
}

void Aplicacao3D::desenharInterface()
{
    // Título do lado esquerdo
    CV::color(1, 1, 0);
    CV::text(10, 30, "EDICAO DE CURVA");
    
    // Título do lado direito
    CV::color(1, 1, 0);
    CV::text(metadeLargura + 10, 30, "VISUALIZACAO 3D");
    
    // Borda da área de visualização 3D
    CV::color(0.5f, 0.5f, 0.5f);
    CV::rect(metadeLargura + 10, 50, larguraTela - 10, alturaTela - 50);
    
    // Instruções do lado esquerdo
    CV::color(1, 1, 1);
    CV::text(15, alturaTela - 150, "A: Adicionar ponto");
    CV::text(15, alturaTela - 130, "D: Deletar ultimo ponto");
    CV::text(15, alturaTela - 110, "Clique/Arraste: Mover ponto");
    
    // Informações da curva
    if (curva) {
        char buffer[100];
        sprintf(buffer, "Pontos de controle: %d", curva->getNumPontosControle());
        CV::text(15, alturaTela - 80, buffer);
        sprintf(buffer, "Pontos da curva: %d", curva->getNumPontosCurva());
        CV::text(15, alturaTela - 60, buffer);
    }
    
    // Instruções do lado direito
    CV::color(1, 1, 1);
    CV::text(metadeLargura + 15, alturaTela - 150, "Mouse: Rotacionar objeto");
    CV::text(metadeLargura + 15, alturaTela - 130, "I/O: Zoom in/out");
    CV::text(metadeLargura + 15, alturaTela - 110, "N: Mostrar normais");
    CV::text(metadeLargura + 15, alturaTela - 90, "P: Alternar projecao");
    
    // Informações do objeto 3D
    if (objeto) {
        char buffer[100];
        sprintf(buffer, "Triangulos: %d", objeto->getNumTriangulos());
        CV::text(metadeLargura + 15, alturaTela - 60, buffer);
        
        CV::color(0, 1, 1);
        CV::text(metadeLargura + 15, alturaTela - 40, objeto->getProjecaoPerspectiva() ? "Perspectiva" : "Ortografica");
    }
    
    // Mensagem se não há pontos suficientes
    if (!curva || curva->getNumPontosControle() < 2) {
        CV::color(0.7f, 0.7f, 0.7f);
        CV::text(metadeLargura + (metadeLargura / 2) - 50, alturaTela / 2, "Adicione pontos");
        CV::text(metadeLargura + (metadeLargura / 2) - 60, (alturaTela / 2) - 20, "na curva para ver");
        CV::text(metadeLargura + (metadeLargura / 2) - 40, (alturaTela / 2) - 40, "o objeto 3D");
    }
}

void Aplicacao3D::onMouseClick(int button, int state, int x, int y)
{
    Vector2 posicaoMouse(x, y);
    
    if (state == 0) { // Mouse pressionado
        if (mouseNaEdicao(x)) {
            // Lado esquerdo - edição de curva
            if (curva) {
                int pontoProximo = curva->pontoProximoControle(posicaoMouse);
                if (pontoProximo != -1) {
                    arrastandoPonto = true;
                    pontoSendoArrastado = pontoProximo;
                } else {
                    // Clique em área vazia - não adiciona ponto automaticamente
                    // Use tecla 'A' para adicionar pontos
                }
            }
        } else {
            // Lado direito - visualização 3D
            rotacionandoObjeto = true;
            ultimaPosicaoMouse = posicaoMouse;
        }
    } else { // Mouse liberado
        arrastandoPonto = false;
        pontoSendoArrastado = -1;
        rotacionandoObjeto = false;
    }
}

void Aplicacao3D::onMouseMove(int x, int y)
{
    Vector2 posicaoMouse(x, y);
    
    if (mouseNaEdicao(x) && arrastandoPonto) {
        // Arrastar ponto de controle no lado esquerdo
        if (curva && pontoSendoArrastado != -1) {
            curva->moverPontoControle(pontoSendoArrastado, posicaoMouse);
        }
    } else if (mouseNaVisualizacao(x) && rotacionandoObjeto) {
        // Rotacionar objeto 3D no lado direito
        if (objeto) {
            Vector2 delta = posicaoMouse - ultimaPosicaoMouse;
            objeto->rotacionar(delta.y * 0.01f, delta.x * 0.01f, 0);
            ultimaPosicaoMouse = posicaoMouse;
        }
    }
}

void Aplicacao3D::onKeyboard(int key)
{
    switch (key) {
        case 'a':
        case 'A':
            // Adicionar ponto de controle no centro da área de edição
            if (curva) {
                Vector2 novoPonto(metadeLargura / 2, alturaTela / 2);
                curva->adicionarPontoControle(novoPonto);
            }
            break;
            
        case 'd':
        case 'D':
            // Deletar último ponto de controle
            if (curva && curva->getNumPontosControle() > 0) {
                curva->removerPontoControle(curva->getNumPontosControle() - 1);
            }
            break;
            
        case 'i':
        case 'I':
            // Zoom in
            if (objeto) {
                objeto->definirEscala(objeto->getEscala() * 1.1f);
            }
            break;
            
        case 'o':
        case 'O':
            // Zoom out
            if (objeto) {
                objeto->definirEscala(objeto->getEscala() * 0.9f);
            }
            break;
            
        case 'n':
        case 'N':
            // Alternar exibição de normais
            if (objeto) {
                objeto->mostrarVetoresNormais(!objeto->getMostrarNormais());
            }
            break;
            
        case 'p':
        case 'P':
            // Alternar projeção perspectiva/ortográfica
            if (objeto) {
                objeto->definirProjecaoPerspectiva(!objeto->getProjecaoPerspectiva());
            }
            break;
            
        case 'r':
        case 'R':
            // Resetar rotação e escala do objeto
            if (objeto) {
                objeto->resetarTransformacoes();
            }
            break;
            
        case 27: // ESC
            exit(0);
            break;
    }
}
