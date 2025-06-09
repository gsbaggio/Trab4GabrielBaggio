

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"
#include "Aplicacao3D.h"

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1080, screenHeight = 720;

Aplicacao3D *app = NULL;

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
    app->atualizar();
    app->desenhar();
   
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
    app->onKeyboard(key);
   
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
    // nao precisou de nada nessa funcao
}

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    printf("Mouse: button=%d, state=%d, wheel=%d, direction=%d, x=%d, y=%d\n", button, state, wheel, direction, x, y);
    if (button == 0) { 
       app->onMouseClick(button, state, x, y);
    } else {
       app->onMouseMove(x, y);
    }
}

int main(void)
{
   app = new Aplicacao3D();
   app->definirDimensoesTela(screenWidth, screenHeight);
   app->inicializar();

   CV::init(&screenWidth, &screenHeight, "B.L.E.N.D.E.R - Baggio Lender, visualizar curvas Bezier em 3D com sweep rotacional e translacional");
   CV::run();
   
   delete app;
   return 0;
}
