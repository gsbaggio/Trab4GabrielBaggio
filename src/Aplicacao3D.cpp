/*
aplicacao3D.cpp - implementa a aplicacao geral do codigo, controla o menu e a chamada de outras classes
o lado esquerdo permite editar pontos de controle de uma curva bezier em tempo real
o lado direito mostra a visualizacao 3D do objeto gerado pelo sweep rotacional da curva ao redor do eixo Y
*/

#include "Aplicacao3D.h"
#include "gl_canvas2d.h"
#include <stdio.h>

Aplicacao3D::Aplicacao3D()
{
    // inicializar variaveis de controle de interacao
    arrastandoPonto = false;
    pontoSendoArrastado = -1;
    rotacionandoObjeto = false;
    
    // definir dimensoes da tela
    larguraTela = 1080;
    alturaTela = 720;
    metadeLargura = larguraTela / 2;
    
    // inicializar posicao do mouse no centro da tela
    posicaoAtualMouse = Vector2(metadeLargura / 2, alturaTela / 2);
    
    // criar curva bezier e objeto 3D associado
    curva = new CurvaBezier();
    objeto = new Objeto3D(curva);
    
    // calcular centro da area de edicao para sistema de coordenadas centralizado
    int centroX = (10 + metadeLargura - 10) / 2;  // centro horizontal da area de edicao
    int centroY = (50 + alturaTela - 50) / 2;     // centro vertical da area de edicao
    
    // adicionar pontos de exemplo para formar uma curva de vaso
    // todos os pontos ficam a direita do eixo Y para gerar revolucao
    curva->adicionarPontoControle(Vector2(centroX + 100, centroY + 200));   // base (abaixo e a direita)
    curva->adicionarPontoControle(Vector2(centroX + 160, centroY + 120));   // corpo inferior (abaixo e a direita)
    curva->adicionarPontoControle(Vector2(centroX + 120, centroY - 60));    // corpo medio (acima e a direita)
    curva->adicionarPontoControle(Vector2(centroX + 80, centroY - 160));    // gargalo (acima e a direita)
    curva->adicionarPontoControle(Vector2(centroX + 100, centroY - 300));   // topo (acima e a direita)
}

Aplicacao3D::~Aplicacao3D()
{
    // liberar memoria alocada
    delete curva;
    delete objeto;
}

void Aplicacao3D::inicializar()
{
    // configurar parametros iniciais do objeto 3D
    objeto->definirProjecaoPerspectiva(false); // iniciar em projecao ortografica
    objeto->definirDivisoesRotacao(10);        // definir 10 divisoes para a revolucao
}

void Aplicacao3D::definirDimensoesTela(int largura, int altura)
{
    // atualizar dimensoes da tela
    larguraTela = largura;
    alturaTela = altura;
    metadeLargura = largura / 2;
    
    // inicializar framebuffer para renderizacao 3D (metade direita da tela)
    int largura3D = metadeLargura - 20; // descontar bordas
    int altura3D = alturaTela - 100;    // descontar bordas superior e inferior
    objeto->inicializarFramebuffer(largura3D, altura3D);
}

bool Aplicacao3D::mouseNaEdicao(int x)
{
    // verificar se o mouse esta na area de edicao (lado esquerdo)
    return x < metadeLargura;
}

bool Aplicacao3D::mouseNaVisualizacao(int x)
{
    // verificar se o mouse esta na area de visualizacao 3D (lado direito)
    return x >= metadeLargura;
}

void Aplicacao3D::atualizar()
{
    // atualizar geometria do objeto 3D quando a curva for modificada
    objeto->atualizarObjeto();
    
}

void Aplicacao3D::desenhar()
{
    // limpar tela com cor cinza escuro
    CV::clear(0.1f, 0.1f, 0.1f);
    
    // primeiro: desenhar a projecao 3D como fundo
    desenharVisualizacao3D();
    
    // segundo: mascarar as bordas para limitar a area 3D
    CV::color(0.1f, 0.1f, 0.1f); // cor de fundo para mascarar
    
    // mascarar toda a area esquerda da tela (onde nao deve aparecer o objeto 3D)
    CV::rectFill(0, 0, metadeLargura, alturaTela);
    
    // mascarar bordas da area 3D (lado direito)
    CV::rectFill(metadeLargura, 0, larguraTela, 50);                    // borda superior
    CV::rectFill(metadeLargura, alturaTela - 50, larguraTela, alturaTela); // borda inferior
    CV::rectFill(metadeLargura, 0, metadeLargura + 10, alturaTela);      // borda esquerda
    CV::rectFill(larguraTela - 10, 0, larguraTela, alturaTela);          // borda direita
    
    // terceiro: desenhar elementos da interface por cima das mascaras
    // desenhar linha divisoria entre as duas areas
    CV::color(0.5f, 0.5f, 0.5f);
    CV::line(metadeLargura, 0, metadeLargura, alturaTela);
    
    // desenhar lado esquerdo - edicao de curva
    desenharEdicaoCurva();
    
    // desenhar instrucoes e informacoes da interface
    desenharInterface();
}

void Aplicacao3D::desenharEdicaoCurva()
{
    // desenhar area de trabalho da curva com fundo cinza
    CV::color(0.2f, 0.2f, 0.2f);
    CV::rectFill(10, 50, metadeLargura - 10, alturaTela - 50);
    CV::color(0.5f, 0.5f, 0.5f);
    CV::rect(10, 50, metadeLargura - 10, alturaTela - 50);
    
    // calcular centro da area de edicao para o sistema de coordenadas
    int centroX = (10 + metadeLargura - 10) / 2;  // centro horizontal da area de edicao
    int centroY = (50 + alturaTela - 50) / 2;     // centro vertical da area de edicao
    
    // desenhar eixos cartesianos
    CV::color(0.7f, 0.7f, 0.7f);  // cinza claro para os eixos
    
    // eixo X (horizontal) - atravessa toda a area de edicao
    CV::line(10, centroY, metadeLargura - 10, centroY);
    
    // eixo Y (vertical) - atravessa toda a area de edicao  
    CV::line(centroX, 50, centroX, alturaTela - 50);
    
    // desenhar marcacoes nos eixos para referencia
    CV::color(0.6f, 0.6f, 0.6f);  // cinza um pouco mais escuro para marcacoes
    
    // marcacoes no eixo X
    for (int i = -3; i <= 3; i++) {
        if (i != 0) { // nao desenhar marcacao no centro
            int x = centroX + i * 40;
            if (x > 10 && x < metadeLargura - 10) {
                CV::line(x, centroY - 3, x, centroY + 3);
            }
        }
    }
    
    // marcacoes no eixo Y
    for (int i = -3; i <= 3; i++) {
        if (i != 0) { // nao desenhar marcacao no centro
            int y = centroY + i * 40;
            if (y > 50 && y < alturaTela - 50) {
                CV::line(centroX - 3, y, centroX + 3, y);
            }
        }
    }
    
    // indicar origem com um pequeno circulo branco
    CV::color(1.0f, 1.0f, 1.0f);  // branco para destacar a origem
    CV::circleFill(centroX, centroY, 3, 8);
    
    // desenhar curva bezier e seus elementos de controle
    curva->desenharPoligonControle(); // linhas conectando pontos de controle
    curva->desenharCurva();           // curva bezier interpolada
    curva->desenharPontosControle();  // pontos de controle editaveis
    
}

void Aplicacao3D::desenharVisualizacao3D()
{
    if (objeto->getModoWireframe()) {
        // renderizacao em wireframe com translacao para centro da area 3D
        CV::translate(metadeLargura + (metadeLargura / 2), alturaTela / 2);
        objeto->desenhar();
        if (objeto->getMostrarNormais()) {
            objeto->desenharNormais(); // desenhar vetores normais se habilitado
        }
        CV::translate(0, 0); // resetar translacao
    } else {
        // renderizacao preenchida - desenhar framebuffer diretamente na tela
        if (objeto->getFramebuffer()) {
            int largura3D = objeto->getFramebuffer()->getLargura();
            int altura3D = objeto->getFramebuffer()->getAltura();
                
            // calcular posicao para centralizar framebuffer na area 3D
            int offsetX = metadeLargura + 10 + (metadeLargura - 20 - largura3D) / 2;
            int offsetY = 50 + (alturaTela - 100 - altura3D) / 2;
                
            // renderizar para o framebuffer (popula o framebuffer com a cena)
            CV::translate(metadeLargura + (metadeLargura / 2), alturaTela / 2);
            objeto->desenhar(); // renderiza para framebuffer mas nao desenha ainda
            CV::translate(0, 0); // resetar translacao
                
            // agora desenhar o framebuffer na posicao correta da tela
            objeto->getFramebuffer()->desenharNaTela(offsetX, offsetY);
        }
    }
    
}

void Aplicacao3D::desenharInterface()
{
    // titulo do lado esquerdo (edicao de curva)
    CV::color(1, 1, 0);
    CV::text(10, 30, "EDICAO DE CURVA");
    
    // titulo do lado direito (visualizacao 3D)
    CV::color(1, 1, 0);
    CV::text(metadeLargura + 10, 30, "VISUALIZACAO 3D");
    
    // borda da area de visualizacao 3D
    CV::color(0.5f, 0.5f, 0.5f);
    CV::rect(metadeLargura + 10, 50, larguraTela - 10, alturaTela - 50);
    
    // instrucoes para edicao de curva (lado esquerdo)
    CV::color(1, 1, 1);
    CV::text(15, 70, "A: Adicionar ponto");
    CV::text(15, 90, "D: Deletar ultimo ponto (min. 4)");
    CV::text(15, 110, "Clique/Arraste: Mover ponto");
    CV::text(15, 130, "J/K: Diminuir/Aumentar pontos de amostragem");
    
    // informacoes sobre a curva atual
    char buffer[100];
    sprintf(buffer, "Pontos de controle: %d", curva->getNumPontosControle());
    CV::text(15, 150, buffer);
        
    sprintf(buffer, "Pontos de amostragem: %d", curva->getNumPontosCurva());
    CV::text(15, 170, buffer);
    
    
    // instrucoes para visualizacao 3D (lado direito)
    CV::color(1, 1, 1);
    CV::text(metadeLargura + 15, 210, "J/K: Diminuir/Aumentar divisoes de rotacao");
    CV::text(metadeLargura + 15, 190, "Q/E: Sweep p/ baixo/cima");
    CV::text(metadeLargura + 15, 170, "Setas: Transladar objeto");
    CV::text(metadeLargura + 15, 150, "Clique/Arraste: Rotacionar objeto");
    CV::text(metadeLargura + 15, 130, "I/O: Zoom in/out");
    CV::text(metadeLargura + 15, 110, "N: Mostrar normais");
    CV::text(metadeLargura + 15, 90, "P: Alternar projecao");
    
    // comandos especiais destacados em amarelo
    CV::color(1, 1, 0);
    CV::text(metadeLargura + 15, 290, "W: Wireframe/Filled");
    CV::text(metadeLargura + 15, 310, "L: Luz esq.atras/dir.frente");
    CV::color(1, 1, 1);
    
    // informacoes sobre o objeto 3D atual
    sprintf(buffer, "Triangulos: %d", objeto->getNumTriangulos());
    CV::text(metadeLargura + 15, 230, buffer);
        
    sprintf(buffer, "Divisoes rotacao: %d", objeto->getNumDivisoesRotacao());
    CV::text(metadeLargura + 15, 250, buffer);
        
    sprintf(buffer, "Sweep translacional: %.1f", objeto->getSweepTranslacional());
    CV::text(metadeLargura + 15, 270, buffer);
        
    // tipo de projecao em destaque
    CV::color(0, 1, 1);
    CV::text(metadeLargura + 15, 70, objeto->getProjecaoPerspectiva() ? "Perspectiva" : "Ortografica");
    
    
}

void Aplicacao3D::onMouseClick(int button, int state, int x, int y)
{
    Vector2 posicaoMouse(x, y);
    posicaoAtualMouse = posicaoMouse; // atualizar posicao atual do mouse
    
    if (state == 0) { // mouse pressionado
        if (mouseNaEdicao(x)) {
            // lado esquerdo - edicao de curva
            // verificar se clicou proximo a algum ponto de controle
            int pontoProximo = curva->pontoProximoControle(posicaoMouse);
            if (pontoProximo != -1) {
                // iniciar arraste do ponto encontrado
                arrastandoPonto = true;
                pontoSendoArrastado = pontoProximo;
            }
            
        } else {
            // lado direito - visualizacao 3D
            // iniciar rotacao do objeto
            rotacionandoObjeto = true;
            ultimaPosicaoMouse = posicaoMouse;
        }
    } else { // mouse liberado
        // parar todas as operacoes de arraste
        arrastandoPonto = false;
        pontoSendoArrastado = -1;
        rotacionandoObjeto = false;
    }
}

void Aplicacao3D::onMouseMove(int x, int y)
{
    Vector2 posicaoMouse(x, y);
    posicaoAtualMouse = posicaoMouse; // sempre atualizar posicao atual
    
    if (mouseNaEdicao(x) && arrastandoPonto) {
        // arrastar ponto de controle no lado esquerdo
        if (pontoSendoArrastado != -1) {
            curva->moverPontoControle(pontoSendoArrastado, posicaoMouse);
        }
    } else if (mouseNaVisualizacao(x) && rotacionandoObjeto) {
        // rotacionar objeto 3D no lado direito
        // calcular delta do movimento do mouse
        Vector2 delta = posicaoMouse - ultimaPosicaoMouse;
        // aplicar rotacao baseada no movimento (sensibilidade 0.01)
        objeto->rotacionar(delta.y * 0.01f, delta.x * 0.01f, 0);
        ultimaPosicaoMouse = posicaoMouse;
        
    }
}

void Aplicacao3D::onKeyboard(int key)
{
    float incrementoAtual;
    switch (key) {
        case 'a':
        case 'A':
            // adicionar ponto de controle na posicao do mouse
            if (mouseNaEdicao(posicaoAtualMouse.x)) {
                // verificar se a posicao esta dentro da area de edicao de curva
                if (posicaoAtualMouse.x >= 10 && posicaoAtualMouse.x <= metadeLargura - 10 &&
                    posicaoAtualMouse.y >= 50 && posicaoAtualMouse.y <= alturaTela - 50) {
                    curva->adicionarPontoControle(posicaoAtualMouse);
                }
            }
            break;
            
        case 'd':
        case 'D':
            // deletar ultimo ponto de controle (manter minimo de 4 pontos)
            if (curva->getNumPontosControle() > 4) {
                curva->removerPontoControle(curva->getNumPontosControle() - 1);
            }
            break;
            
        case 'i':
        case 'I':
            // zoom in - aumentar escala do objeto
            objeto->definirEscala(objeto->getEscala() * 1.1f);
            
            break;
            
        case 'o':
        case 'O':
            // zoom out - diminuir escala do objeto
            objeto->definirEscala(objeto->getEscala() * 0.9f);
            
            break;
            
        case 'n':
        case 'N':
            // alternar exibicao de vetores normais
            objeto->mostrarVetoresNormais(!objeto->getMostrarNormais());
            
            break;
            
        case 'p':
        case 'P':
            // alternar entre projecao perspectiva e ortografica
            objeto->definirProjecaoPerspectiva(!objeto->getProjecaoPerspectiva());
            
            break;
            
        case 'w':
        case 'W':
            // alternar entre modo wireframe e preenchido
            objeto->definirModoWireframe(!objeto->getModoWireframe());
            
            break;
            
        case 'l':
        case 'L':
            // alternar direcao da luz entre esquerda/atras e direita/frente
            static bool luzEsquerdaAtras = true;
            if (luzEsquerdaAtras) {
                // configurar luz a direita na frente
                objeto->definirLuz(Vector3(0.5f, 0.0f, 1.0f), Vector3(1.0f, 1.0f, 1.0f));
            } else {
                // configurar luz a esquerda atras
                objeto->definirLuz(Vector3(-0.5f, 0.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
            }
            luzEsquerdaAtras = !luzEsquerdaAtras;
            
            break;
            
        case 'e':
        case 'E':
            // aumentar sweep translacional (movimento para cima)
            incrementoAtual = objeto->getSweepTranslacional();
            objeto->definirSweepTranslacional(incrementoAtual + 5.0f);
            
            break;
            
        case 'q':
        case 'Q':
            // diminuir sweep translacional (movimento para baixo)
            incrementoAtual = objeto->getSweepTranslacional();
            objeto->definirSweepTranslacional(incrementoAtual - 5.0f);
            
            break;
            
        case 't':
        case 'T':
            // resetar todas as transformacoes do objeto (rotacao e escala)
            objeto->resetarTransformacoes();
            
            break;
            
        case 'j':
        case 'J':
            // comportamento diferente dependendo da area do mouse
            if (mouseNaEdicao(posicaoAtualMouse.x)) {
                // lado esquerdo - diminuir pontos da curva (minimo 10)
                int pontosAtuais = curva->getNumPontosCurva();
                if (pontosAtuais > 10) {
                    curva->definirNumeropontos(pontosAtuais - 10);
                }
                
            } else {
                // lado direito - diminuir divisoes de rotacao (minimo 8)
                int divisoesAtuais = objeto->getNumDivisoesRotacao();
                if (divisoesAtuais > 8) {
                    objeto->definirDivisoesRotacao(divisoesAtuais / 2);
                }
                
            }
            break;
            
        case 'k':
        case 'K':
            // comportamento diferente dependendo da area do mouse
            if (mouseNaEdicao(posicaoAtualMouse.x)) {
                // lado esquerdo - aumentar pontos da curva (maximo 200)
                int pontosAtuais = curva->getNumPontosCurva();
                if (pontosAtuais < 200) {
                    curva->definirNumeropontos(pontosAtuais + 10);
                }
                
            } else {
                // lado direito - aumentar divisoes de rotacao (maximo 80)
                int divisoesAtuais = objeto->getNumDivisoesRotacao();
                if (divisoesAtuais < 80) {
                    objeto->definirDivisoesRotacao(divisoesAtuais * 2);
                }
                
            }
            break;
            
        // teclas de seta para translacao do objeto 3D
        case 201: // seta para cima
            objeto->transladar(0, -10, 0); // mover para cima (Y negativo)
            
            break;
            
        case 203: // seta para baixo
            objeto->transladar(0, 10, 0); // mover para baixo (Y positivo)
            
            break;
            
        case 200: // seta para esquerda
            objeto->transladar(-10, 0, 0); // mover para esquerda (X negativo)
            
            break;
            
        case 202: // seta para direita
            objeto->transladar(10, 0, 0); // mover para direita (X positivo)
            
            break;
            
        case 27: // tecla ESC - sair da aplicacao
            exit(0);
            break;
    }
}
