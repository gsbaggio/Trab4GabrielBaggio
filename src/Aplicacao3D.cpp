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
    posicaoAtualMouse = Vector2(metadeLargura / 2, alturaTela / 2); // Inicializar no centro
    
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
    objeto->definirProjecaoPerspectiva(false); // Iniciar em ortográfica
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
    if (curva->getNumPontosControle() >= 2) {
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
    
    // Mascarar TODA a área esquerda da tela (onde não deve aparecer o objeto 3D)
    CV::rectFill(0, 0, metadeLargura, alturaTela);
    
    // Mascarar borda superior da área 3D (lado direito)
    CV::rectFill(metadeLargura, 0, larguraTela, 50);
    
    // Mascarar borda inferior da área 3D (lado direito)
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
    CV::rect(metadeLargura + 10, 50, larguraTela - 10, alturaTela - 50);    // Instruções do lado esquerdo
    CV::color(1, 1, 1);
    CV::text(15, 70, "A: Adicionar ponto");
    CV::text(15, 90, "D: Deletar ultimo ponto (min. 2)");
    CV::text(15, 110, "Clique/Arraste: Mover ponto");
    CV::text(15, 130, "J/K: Diminuir/Aumentar pontos curva");
    
    // Informações da curva
    if (curva) {
        char buffer[100];
        sprintf(buffer, "Pontos de controle: %d", curva->getNumPontosControle());
        CV::text(15, 150, buffer);
        
        sprintf(buffer, "Pontos de amostragem: %d", curva->getNumPontosCurva());
        CV::text(15, 170, buffer);
    }    // Instruções do lado direito
    CV::color(1, 1, 1);
    CV::text(metadeLargura + 15, 190, "Setas: Transladar objeto");
    CV::text(metadeLargura + 15, 170, "Mouse: Rotacionar objeto");
    CV::text(metadeLargura + 15, 150, "I/O: Zoom in/out");
    CV::text(metadeLargura + 15, 130, "N: Mostrar normais");
    CV::text(metadeLargura + 15, 110, "P: Alternar projecao");
    CV::text(metadeLargura + 15, 90, "J/K: Diminuir/Aumentar divisoes de rotacao");
    
    // Informações do objeto 3D
    if (objeto) {
        char buffer[100];
        sprintf(buffer, "Triangulos: %d", objeto->getNumTriangulos());
        CV::text(metadeLargura + 15, 210, buffer);
        
        sprintf(buffer, "Vertices: %d", objeto->getNumVertices());
        CV::text(metadeLargura + 15, 230, buffer);
        
        sprintf(buffer, "Divisoes rotacao: %d", objeto->getNumDivisoesRotacao());
        CV::text(metadeLargura + 15, 250, buffer);
        
        CV::color(0, 1, 1);
        CV::text(metadeLargura + 15, 70, objeto->getProjecaoPerspectiva() ? "Perspectiva" : "Ortografica");
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
    posicaoAtualMouse = posicaoMouse; // Atualizar posição atual
    
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
    posicaoAtualMouse = posicaoMouse; // Sempre atualizar posição atual
    
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
    switch (key) {        case 'a':
        case 'A':
            // Adicionar ponto de controle na posição do mouse (apenas se estiver na área de edição)
            if (curva && mouseNaEdicao(posicaoAtualMouse.x)) {
                // Verificar se a posição está dentro da área de edição de curva
                if (posicaoAtualMouse.x >= 10 && posicaoAtualMouse.x <= metadeLargura - 10 &&
                    posicaoAtualMouse.y >= 50 && posicaoAtualMouse.y <= alturaTela - 50) {
                    curva->adicionarPontoControle(posicaoAtualMouse);
                }
            }
            break;
              case 'd':
        case 'D':
            // Deletar último ponto de controle (apenas se tiver mais de 2 pontos)
            if (curva && curva->getNumPontosControle() > 2) {
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
            break;        case 'r':
        case 'R':
            // Resetar rotação e escala do objeto
            if (objeto) {
                objeto->resetarTransformacoes();
            }
            break;
              case 'j':
        case 'J':
            if (mouseNaEdicao(posicaoAtualMouse.x)) {
                // Lado esquerdo - diminuir pontos da curva (mínimo 10)
                if (curva) {
                    int pontosAtuais = curva->getNumPontosCurva();
                    if (pontosAtuais > 10) {
                        curva->definirNumeropontos(pontosAtuais - 10);
                    }
                }
            } else {
                // Lado direito - dividir número de faces por 2 (mínimo 8)
                if (objeto) {
                    int divisoesAtuais = objeto->getNumDivisoesRotacao();
                    if (divisoesAtuais > 8) {
                        objeto->definirDivisoesRotacao(divisoesAtuais / 2);
                    }
                }
            }
            break;
            
        case 'k':
        case 'K':
            if (mouseNaEdicao(posicaoAtualMouse.x)) {
                // Lado esquerdo - aumentar pontos da curva (máximo 200)
                if (curva) {
                    int pontosAtuais = curva->getNumPontosCurva();
                    if (pontosAtuais < 200) {
                        curva->definirNumeropontos(pontosAtuais + 10);
                    }
                }
            } else {
                // Lado direito - multiplicar número de faces por 2 (máximo 80)
                if (objeto) {
                    int divisoesAtuais = objeto->getNumDivisoesRotacao();
                    if (divisoesAtuais < 80) {
                        objeto->definirDivisoesRotacao(divisoesAtuais * 2);
                    }
                }
            }
            break;
            
        // Setas do teclado para translação
        case 201: // Seta para cima
            if (objeto) {
                objeto->transladar(0, -10, 0); // Move para cima (Y negativo)
            }
            break;
            
        case 203: // Seta para baixo
            if (objeto) {
                objeto->transladar(0, 10, 0); // Move para baixo (Y positivo)
            }
            break;
            
        case 200: // Seta para esquerda
            if (objeto) {
                objeto->transladar(-10, 0, 0); // Move para esquerda (X negativo)
            }
            break;
            
        case 202: // Seta para direita
            if (objeto) {
                objeto->transladar(10, 0, 0); // Move para direita (X positivo)
            }
            break;
            
        case 27: // ESC
            exit(0);
            break;
    }
}
