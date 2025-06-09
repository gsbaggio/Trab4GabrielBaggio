/*

Nome: Gabriel Souza Baggio

Quesitos Implementados:

1 - Usuário pode informar pontos de controle. Mínimo é 4 pontos, mas podem ser adicionados mais.
2 - É gerado uma curva de Bezier nesses pontos.
3 - Essa curva é rotacionada com sweep rotacional e exibida em uma área 3D.
4 -  Os pontos de controle podem ser editados a qualquer momento e refletem instantaneamente na curva.

Requisitos básicos implementados:

1 - Modelagem do objeto com opção de rotação e translação, para geração de objetos como
garrafas, donuts e molas. A rotação do objeto é feito em relação a origem, sendo feita
ao clicar e arrastar o mouse. Rotação ao mexer o mouse horizontalmente é feita em torno
do eixo Y, enquanto mexer verticalmente é em torno do eixo X ou Z. A translação do objeto
é feita com as setas do teclado, movendo o objeto para cima, baixo, esquerda e direita.

2 - O sweep translacional é alterado com as teclas 'Q' e 'E', permitindo molas. Para cada
face do sweep rotacional, é acrescido um deslocamento vertical (eixo Y) baseado no sweep.

3 - Visualização ortográfica e perspectiva (sob vários ângulos) em wireframe, com uso de
triângulos. Com a tecla 'P', pode-se alternar entre projeção ortográfica e perspectiva.

4 - Parametrização do número de pontos e faces do objeto gerado. Apertar 'J' ou 'K' com
o mouse no lado da curva altera a quantidade de pontos amostrados na curva, enquanto
apertar 'J' ou 'K' com o mouse no lado do objeto altera a quantidade de faces do objeto,
ou seja, o numero de rotações que foram feitas no sweep rotacional.

5 - Apertar 'I' ou 'O' altera o zoom do objeto 3D, aproximando ou afastando a câmera.

Requisitos avançados implementados:

1 - Pode-se ver os vetores normais do objeto 3D, com a tecla 'N'.

2 - Pode-se adicionar mais de 4 pontos de controle, com a tecla 'A'. A tecla
'D' remove o último ponto de controle.

3 - Preenchimento de polígonos com z-buffer e scanline com Iluminação por pixel. Apertar
a tecla 'W' alterna entre o modo wireframe e o modo preenchido. A direção da luz pode ser
alterada com a tecla 'L'. 

*/

/*
Manual de uso:


*/

// arquivo main do trabalho. Responsável apenas pela inicialização das classes e chamadas de input.
// render chama apenas as funções de renderização em outros códigos

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"
#include "Aplicacao3D.h"

//largura e altura inicial da tela . alteram com o redimensionamento de tela.
int screenWidth = 1080, screenHeight = 720;

Aplicacao3D *app = NULL;

//função chamada continuamente. deve-se controlar o que desenhar por meio de variáveis globais
//todos os comandos para desenho na canvas devem ser chamados dentro da render().
//deve-se manter essa função com poucas linhas de código.
void render()
{
    app->atualizar();
    app->desenhar();
}

//função chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
    app->onKeyboard(key);
   
}

//função chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
    // não precisou de nada nessa função
}

//função para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    if (button == 0) { // quando o botão esquerdo do mouse é pressionado ou solto
       app->onMouseClick(button, state, x, y);
    } else if (button == -2 && state == -2 && wheel == -2 && direction == -2) { // mouse movimentando apenas
       app->onMouseMove(x, y);
    }
}

int main(void)
{
   app = new Aplicacao3D();
   app->definirDimensoesTela(screenWidth, screenHeight);
   app->inicializar();

   CV::init(&screenWidth, &screenHeight, "G.B.L.E.N.D.E.R - Gabriel Baggio Lender, visualizar curvas Bezier em 3D com sweep rotacional e translacional");
   CV::run();
   
   delete app;
   return 0;
}
