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
      // Calcular centro da área de edição para sistema de coordenadas centralizado
    int centroX = (10 + metadeLargura - 10) / 2;  // Centro horizontal da área de edição
    int centroY = (50 + alturaTela - 50) / 2;     // Centro vertical da área de edição
    
    // Adicionar alguns pontos de exemplo com coordenadas centralizadas
    // Pontos à direita do eixo Y, alguns acima e outros abaixo do eixo X
    curva->adicionarPontoControle(Vector2(centroX + 100, centroY + 200));   // Base (abaixo e à direita)
    curva->adicionarPontoControle(Vector2(centroX + 160, centroY + 120));   // Corpo inferior (abaixo e à direita)
    curva->adicionarPontoControle(Vector2(centroX + 120, centroY - 60));   // Corpo médio (acima e à direita)
    curva->adicionarPontoControle(Vector2(centroX + 80, centroY - 160));   // Gargalo (acima e à direita)
    curva->adicionarPontoControle(Vector2(centroX + 100, centroY - 300));  // Topo (acima e à direita)
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
    objeto->definirDivisoesRotacao(10); // Ajustado para 10 divisões iniciais
}

void Aplicacao3D::definirDimensoesTela(int largura, int altura)
{
    larguraTela = largura;
    alturaTela = altura;
    metadeLargura = largura / 2;
    
    // Initialize framebuffer for 3D rendering area (right half of screen)
    int largura3D = metadeLargura - 20; // Account for borders
    int altura3D = alturaTela - 100;    // Account for top/bottom borders
    objeto->inicializarFramebuffer(largura3D, altura3D);
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
    
    // Calcular centro da área de edição para o sistema de coordenadas
    int centroX = (10 + metadeLargura - 10) / 2;  // Centro horizontal da área de edição
    int centroY = (50 + alturaTela - 50) / 2;     // Centro vertical da área de edição
    
    // Desenhar eixos cartesianos
    CV::color(0.7f, 0.7f, 0.7f);  // Cinza claro para os eixos
    
    // Eixo X (horizontal) - atravessa toda a área de edição
    CV::line(10, centroY, metadeLargura - 10, centroY);
    
    // Eixo Y (vertical) - atravessa toda a área de edição  
    CV::line(centroX, 50, centroX, alturaTela - 50);
    
    // Desenhar marcações nos eixos (opcional)
    CV::color(0.6f, 0.6f, 0.6f);  // Cinza um pouco mais escuro para marcações
    
    // Marcações no eixo X
    for (int i = -3; i <= 3; i++) {
        if (i != 0) { // Não desenhar marcação no centro
            int x = centroX + i * 40;
            if (x > 10 && x < metadeLargura - 10) {
                CV::line(x, centroY - 3, x, centroY + 3);
            }
        }
    }
    
    // Marcações no eixo Y
    for (int i = -3; i <= 3; i++) {
        if (i != 0) { // Não desenhar marcação no centro
            int y = centroY + i * 40;
            if (y > 50 && y < alturaTela - 50) {
                CV::line(centroX - 3, y, centroX + 3, y);
            }
        }
    }
    
    // Indicar origem com um pequeno círculo
    CV::color(1.0f, 1.0f, 1.0f);  // Branco para destacar a origem
    CV::circleFill(centroX, centroY, 3, 8);
    
    // Desenhar curva e pontos de controle
    if (curva) {
        curva->desenharPoligonControle();
        curva->desenharCurva();
        curva->desenharPontosControle();
    }
}

void Aplicacao3D::desenharVisualizacao3D()
{
    if (objeto && curva->getNumPontosControle() >= 2) {
        if (objeto->getModoWireframe()) {
            // Wireframe rendering with translation
            CV::translate(metadeLargura + (metadeLargura / 2), alturaTela / 2);
            objeto->desenhar();
            if (objeto->getMostrarNormais()) {
                objeto->desenharNormais();
            }
            CV::translate(0, 0); // Reset da translação
        } else {
            // Filled rendering - draw framebuffer directly to screen area
            // First draw the framebuffer with proper positioning
            if (objeto->getFramebuffer()) {
                int largura3D = objeto->getFramebuffer()->getLargura();
                int altura3D = objeto->getFramebuffer()->getAltura();
                
                // Calculate position to center framebuffer in 3D area
                int offsetX = metadeLargura + 10 + (metadeLargura - 20 - largura3D) / 2;
                int offsetY = 50 + (alturaTela - 100 - altura3D) / 2;
                
                // Render to framebuffer (this will populate the framebuffer with the scene)
                CV::translate(metadeLargura + (metadeLargura / 2), alturaTela / 2);
                objeto->desenhar(); // This will render to framebuffer but not draw it yet
                CV::translate(0, 0); // Reset translation
                
                // Now draw the framebuffer at the correct screen position
                objeto->getFramebuffer()->desenharNaTela(offsetX, offsetY);
            }
        }
    }
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
    CV::text(15, 90, "D: Deletar ultimo ponto (min. 4)");
    CV::text(15, 110, "Clique/Arraste: Mover ponto");
    CV::text(15, 130, "J/K: Diminuir/Aumentar pontos curva");
    CV::text(15, 150, "Origem no centro da area");
    CV::text(15, 170, "Eixo X (horizontal) e Y (vertical)");
      // Informações da curva
    if (curva) {
        char buffer[100];
        sprintf(buffer, "Pontos de controle: %d", curva->getNumPontosControle());
        CV::text(15, 190, buffer);
        
        sprintf(buffer, "Pontos de amostragem: %d", curva->getNumPontosCurva());
        CV::text(15, 210, buffer);
    }    // Instruções do lado direito
    CV::color(1, 1, 1);
    CV::text(metadeLargura + 15, 210, "J/K: Diminuir/Aumentar divisoes de rotacao");
    CV::text(metadeLargura + 15, 190, "Q/E: Sweep p/ baixo/cima");
    CV::text(metadeLargura + 15, 170, "Setas: Transladar objeto");
    CV::text(metadeLargura + 15, 150, "Mouse: Rotacionar objeto");
    CV::text(metadeLargura + 15, 130, "I/O: Zoom in/out");
    CV::text(metadeLargura + 15, 110, "N: Mostrar normais");
    CV::text(metadeLargura + 15, 90, "P: Alternar projecao");    CV::color(1, 1, 0);
    CV::text(metadeLargura + 15, 290, "W: Wireframe/Filled");
    CV::text(metadeLargura + 15, 310, "L: Luz esq.atras/dir.frente");
    CV::color(1, 1, 1);
    
    // Informações do objeto 3D
    if (objeto) {
        char buffer[100];
        sprintf(buffer, "Triangulos: %d", objeto->getNumTriangulos());
        CV::text(metadeLargura + 15, 230, buffer);
        
        
        sprintf(buffer, "Divisoes rotacao: %d", objeto->getNumDivisoesRotacao());
        CV::text(metadeLargura + 15, 250, buffer);
        
        sprintf(buffer, "Sweep translacional: %.1f", objeto->getSweepTranslacional());
        CV::text(metadeLargura + 15, 270, buffer);
        
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
            break;        case 'd':
        case 'D':
            // Deletar último ponto de controle (apenas se tiver mais de 4 pontos)
            if (curva && curva->getNumPontosControle() > 4) {
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
            
        case 'w':
        case 'W':
            // Alternar modo wireframe/filled
            if (objeto) {
                objeto->definirModoWireframe(!objeto->getModoWireframe());
            }
            break;        case 'l':
        case 'L':
            // Alternar direção da luz
            if (objeto) {
                static bool luzEsquerdaAtras = true;
                if (luzEsquerdaAtras) {
                    // Luz à direita na frente
                    objeto->definirLuz(Vector3(0.5f, 0.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f));
                } else {
                    // Luz à esquerda atrás
                    objeto->definirLuz(Vector3(-0.5f, 0.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
                }
                luzEsquerdaAtras = !luzEsquerdaAtras;
            }            break;
            
        case 'e':
        case 'E':
            // Aumentar incremento de sweep translacional
            if (objeto) {
                float incrementoAtual = objeto->getSweepTranslacional();
                objeto->definirSweepTranslacional(incrementoAtual + 5.0f);
            }
            break;
            
        case 'q':
        case 'Q':
            // Diminuir incremento de sweep translacional
            if (objeto) {
                float incrementoAtual = objeto->getSweepTranslacional();
                objeto->definirSweepTranslacional(incrementoAtual - 5.0f);
            }            break;
            
        case 't':
        case 'T':
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
