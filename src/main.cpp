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
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().


//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   if (app) {
       app->atualizar();
       app->desenhar();
   }
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   if (app) {
       app->onKeyboard(key);
   }
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   printf("\nLiberou: %d" , key);
}

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;

   if (app) {
       if (button != -2) { // Se não for movimento do mouse
           app->onMouseClick(button, state, x, y);
       } else {
           app->onMouseMove(x, y);
       }
   }
}

int main(void)
{
   app = new Aplicacao3D();
   app->definirDimensoesTela(screenWidth, screenHeight);
   app->inicializar();

   CV::init(&screenWidth, &screenHeight, "Modelagem 3D - Curvas de Bezier e Sweep Rotacional");
   CV::run();
   
   delete app;
   return 0;
}
