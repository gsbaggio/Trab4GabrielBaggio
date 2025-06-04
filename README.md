# Instruções para Compilação e Execução

Este projeto implementa um sistema de modelagem 3D usando curvas de Bézier e sweep rotacional.

## Pré-requisitos

- OpenGL
- GLUT ou FreeGLUT
- Compilador C++ (GCC, MinGW, Visual Studio, etc.)

## Estrutura dos Arquivos

### Arquivos Principais:
- `main.cpp` - Arquivo principal da aplicação
- `Aplicacao3D.h/cpp` - Classe principal que gerencia a aplicação
- `CurvaBezier.h/cpp` - Implementa curvas de Bézier
- `Objeto3D.h/cpp` - Gera objetos 3D através de sweep rotacional
- `Vector2.h` - Classe para vetores 2D
- `Vector3.h` - Classe para vetores 3D
- `gl_canvas2d.h/cpp` - Interface gráfica baseada em OpenGL

## Como Compilar

### Windows (MinGW):
```bash
g++ -o bezier3d src/*.cpp -lopengl32 -lglu32 -lfreeglut -std=c++11
```

### Linux:
```bash
g++ -o bezier3d src/*.cpp -lGL -lGLU -lglut -std=c++11
```

### CodeBlocks:
1. Abra o arquivo `canvasGlut.cbp`
2. Configure as bibliotecas do OpenGL/GLUT no projeto
3. Compile e execute

## Como Usar

### Modo Edição de Curva:
- **Clique**: Adiciona pontos de controle da curva de Bézier
- **Arrastar**: Move pontos de controle existentes
- **Delete**: Remove o último ponto de controle
- **Tab**: Alterna para visualização 3D

### Modo Visualização 3D:
- **Mouse arrastar**: Rotaciona o objeto 3D
- **W/S**: Rotação no eixo X
- **A/D**: Rotação no eixo Y
- **Setas**: Rotação no eixo Z
- **N**: Mostra/oculta vetores normais
- **P**: Alterna entre projeção ortográfica e perspectiva
- **+/-**: Aumenta/diminui número de divisões da rotação
- **R**: Reseta transformações
- **Tab**: Volta para edição de curva

### Funcionalidades:
- ✅ Edição interativa de curvas de Bézier
- ✅ Geração automática de objetos 3D por sweep rotacional
- ✅ Visualização wireframe com triângulos
- ✅ Projeção ortográfica e perspectiva
- ✅ Exibição de vetores normais
- ✅ Rotação e transformação interativa
- ✅ Múltiplas curvas/objetos
- ✅ Interface gráfica intuitiva

## Exemplos de Objetos Gerados

- **Garrafa**: Crie uma curva com formato de garrafa no modo 2D
- **Donut**: Crie uma curva em forma de C
- **Mola**: Crie uma curva ondulada
- **Vaso**: Crie uma curva côncava

## Controles de Interface

### Botões Clicáveis:
- **Modo: Edição/3D**: Alterna entre modos
- **Nova Curva**: Adiciona uma nova curva
- **Wireframe/Solid**: Alterna modo de renderização
- **Normais ON/OFF**: Mostra/oculta normais
- **Perspectiva/Ortográfica**: Alterna projeção

## Troubleshooting

1. **Erro de compilação**: Verifique se as bibliotecas OpenGL/GLUT estão instaladas
2. **Janela não abre**: Verifique drivers de vídeo
3. **Performance baixa**: Reduza o número de divisões da rotação

## Arquitetura do Código

- **Separação de responsabilidades**: Cada classe tem função específica
- **Orientação a objetos**: Uso de classes para organizar funcionalidades
- **Padrão Observer**: Mudanças na curva atualizam automaticamente o 3D
- **Interface responsiva**: Atualizações em tempo real

Este projeto demonstra conceitos avançados de computação gráfica, incluindo curvas paramétricas, geometria 3D, projeções e interfaces interativas.
