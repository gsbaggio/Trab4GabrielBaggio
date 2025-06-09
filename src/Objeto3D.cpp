/* 
implementação da classe Objeto3D que gera e renderiza objetos 3D através de sweep rotacional
suporta tanto renderização wireframe quanto rasterização com z-buffer e iluminação per-pixel
permite transformações 3D como rotação, translação, escala e diferentes tipos de projeção
*/

#include "Objeto3D.h"
#include "gl_canvas2d.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265359
#endif

Objeto3D::Objeto3D(CurvaBezier* curva)
{
    curvaBezier = curva;
    numDivisoesRotacao = 10; // ajustado para 10 divisões iniciais
    sweepTranslacional = 0.0f; // sem incremento vertical inicial
    mostrarNormais = false;
    modoWireframe = true;
    projecaoPerspectiva = false; // já está em ortográfica
    distanciaCamera = 800.0f; // aumentado de 300 para 800 - câmera mais distante
    
    // inicializar sistema de renderização
    framebuffer = nullptr;
    rasterizer = new Rasterizer();
    
    // configurar iluminação padrão - luz à esquerda e atrás
    rasterizer->definirLuz(Vector3(-0.5f, 0.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
    rasterizer->definirCorAmbiente(Vector3(0.2f, 0.2f, 0.2f));
    rasterizer->definirCorMaterial(Vector3(0.4f, 0.6f, 0.8f)); // material azul
    
    // inicializar transformações
    rotacaoX = rotacaoY = rotacaoZ = 0.0f;
    translacaoX = translacaoY = translacaoZ = 0.0f;
    escala = 0.6f; // aumentado de 0.3 para 0.6 - objeto com mais zoom inicial
}

Objeto3D::~Objeto3D()
{
    delete rasterizer;
    delete framebuffer;
}

void Objeto3D::inicializarFramebuffer(int largura, int altura)
{
    delete framebuffer;
    framebuffer = new Framebuffer(largura, altura);
}

void Objeto3D::definirLuz(Vector3 direcao, Vector3 cor)
{
    if (rasterizer) {
        rasterizer->definirLuz(direcao, cor);
    }
}

void Objeto3D::definirCorAmbiente(Vector3 cor)
{
    if (rasterizer) {
        rasterizer->definirCorAmbiente(cor);
    }
}

void Objeto3D::definirCorMaterial(Vector3 cor)
{
    if (rasterizer) {
        rasterizer->definirCorMaterial(cor);
    }
}

void Objeto3D::definirDivisoesRotacao(int divisoes)
{
    numDivisoesRotacao = divisoes;
    atualizarObjeto();
}

void Objeto3D::definirSweepTranslacional(float incremento)
{
    sweepTranslacional = incremento;
    atualizarObjeto();
}

void Objeto3D::mostrarVetoresNormais(bool mostrar)
{
    mostrarNormais = mostrar;
}

void Objeto3D::definirModoWireframe(bool wireframe)
{
    modoWireframe = wireframe;
}

void Objeto3D::definirProjecaoPerspectiva(bool perspectiva)
{
    projecaoPerspectiva = perspectiva;
}

void Objeto3D::rotacionar(float deltaX, float deltaY, float deltaZ)
{
    rotacaoX += deltaX;
    rotacaoY += deltaY;
    rotacaoZ += deltaZ;
}

void Objeto3D::transladar(float deltaX, float deltaY, float deltaZ)
{
    translacaoX += deltaX;
    translacaoY += deltaY;
    translacaoZ += deltaZ;
}

void Objeto3D::definirEscala(float novaEscala)
{
    escala = novaEscala;
}

void Objeto3D::resetarTransformacoes()
{
    rotacaoX = rotacaoY = rotacaoZ = 0.0f;
    translacaoX = translacaoY = translacaoZ = 0.0f;
    escala = 0.6f; // mesma escala inicial aumentada
    distanciaCamera = 800.0f; // mesma distância inicial
}

void Objeto3D::definirDistanciaCamera(float distancia)
{
    distanciaCamera = distancia;
}

Vector3 Objeto3D::transformarPonto(Vector3 ponto)
{
    Vector3 p = ponto;
    
    // aplicar escala
    p = p * escala;
    
    // aplicar rotações usando matrizes de rotação 3D
    // rotação em X
    if (rotacaoX != 0) {
        float cosX = cos(rotacaoX);
        float sinX = sin(rotacaoX);
        float y = p.y * cosX - p.z * sinX;
        float z = p.y * sinX + p.z * cosX;
        p.y = y;
        p.z = z;
    }
    
    // rotação em Y
    if (rotacaoY != 0) {
        float cosY = cos(rotacaoY);
        float sinY = sin(rotacaoY);
        float x = p.x * cosY + p.z * sinY;
        float z = -p.x * sinY + p.z * cosY;
        p.x = x;
        p.z = z;
    }
    
    // rotação em Z
    if (rotacaoZ != 0) {
        float cosZ = cos(rotacaoZ);
        float sinZ = sin(rotacaoZ);
        float x = p.x * cosZ - p.y * sinZ;
        float y = p.x * sinZ + p.y * cosZ;
        p.x = x;
        p.y = y;
    }
    
    // aplicar translação
    p.x += translacaoX;
    p.y += translacaoY;
    p.z += translacaoZ;
    
    return p;
}

Vector2 Objeto3D::projetarPonto(Vector3 ponto)
{
    Vector3 p = transformarPonto(ponto);
    Vector2 resultado;
    
    if (projecaoPerspectiva) {
        // projeção perspectiva com divisão por profundidade
        float fator = distanciaCamera / (distanciaCamera + p.z);
        resultado.x = p.x * fator;
        resultado.y = p.y * fator;
    } else {
        // projeção ortográfica simples
        resultado.x = p.x;
        resultado.y = p.y;
    }
    
    return resultado;
}

Vector2 Objeto3D::projetarPontoFramebuffer(Vector3 ponto)
{
    Vector3 p = transformarPonto(ponto);
    Vector2 resultado;
    
    if (projecaoPerspectiva) {
        // projeção perspectiva
        float fator = distanciaCamera / (distanciaCamera + p.z);
        resultado.x = p.x * fator;
        resultado.y = p.y * fator;
    } else {
        // projeção ortográfica
        resultado.x = p.x;
        resultado.y = p.y;
    }
    
    // converter de coordenadas OpenGL para coordenadas de pixel do framebuffer
    if (framebuffer) {
        // centralizar o objeto no framebuffer e manter mesma orientação Y do wireframe
        resultado.x = (framebuffer->getLargura() / 2) + resultado.x;
        resultado.y = (framebuffer->getAltura() / 2) + resultado.y; // manter mesma direção Y do wireframe
    }
    
    return resultado;
}

Vector3 Objeto3D::transformarNormal(Vector3 normal)
{
    Vector3 n = normal;
    
    // aplicar apenas rotações às normais (sem escala ou translação)
    // rotação em X
    if (rotacaoX != 0) {
        float cosX = cos(rotacaoX);
        float sinX = sin(rotacaoX);
        float y = n.y * cosX - n.z * sinX;
        float z = n.y * sinX + n.z * cosX;
        n.y = y;
        n.z = z;
    }
    
    // rotação em Y
    if (rotacaoY != 0) {
        float cosY = cos(rotacaoY);
        float sinY = sin(rotacaoY);
        float x = n.x * cosY + n.z * sinY;
        float z = -n.x * sinY + n.z * cosY;
        n.x = x;
        n.z = z;
    }
    
    // rotação em Z
    if (rotacaoZ != 0) {
        float cosZ = cos(rotacaoZ);
        float sinZ = sin(rotacaoZ);
        float x = n.x * cosZ - n.y * sinZ;
        float y = n.x * sinZ + n.y * cosZ;
        n.x = x;
        n.y = y;
    }
    
    // normalizar a normal após transformação para manter comprimento unitário
    n.normalize();
    
    return n;
}

void Objeto3D::gerarSweepRotacional()
{
    vertices.clear();
    triangulos.clear();
    
    std::vector<Vector2>& pontosCurva = curvaBezier->getPontosCurva();
    if (pontosCurva.size() < 2) return;
    
    // calcular centro da área de edição para converter coordenadas de tela para coordenadas de mundo 3D
    // estes valores devem corresponder aos mesmos usados em Aplicacao3D.cpp
    int larguraTela = 1080;
    int alturaTela = 720;
    int metadeLargura = larguraTela / 2;
    int centroX = (10 + metadeLargura - 10) / 2;  // centro horizontal da área de edição
    int centroY = (50 + alturaTela - 50) / 2;     // centro vertical da área de edição
    
    // gerar vértices através de rotação ao redor do eixo Y
    float anguloIncremento = 2.0f * PI / numDivisoesRotacao;
    
    for (int i = 0; i <= numDivisoesRotacao; i++) {
        float angulo = i * anguloIncremento;
        float cosA = cos(angulo);
        float sinA = sin(angulo);
        
        // calcular deslocamento vertical para sweep translacional
        // valores positivos fazem crescer para cima, negativos para baixo
        float deslocamentoY = -i * sweepTranslacional;
        
        for (size_t j = 0; j < pontosCurva.size(); j++) {
            Vector2& p = pontosCurva[j];
            
            // converter coordenadas de tela (sistema centralizado) para coordenadas de mundo 3D
            float xMundo = p.x - centroX;   // deslocar origem do centro para obter distância do eixo Y
            float yMundo = -(p.y - centroY); // inverter Y e deslocar origem (sistema de coordenadas padrão)
            
            // para rotação 3D, o raio é a distância absoluta do eixo Y (agora origem está centralizada)
            float raio = fabs(xMundo);
            
            // criar vértice 3D rotacionando ao redor do eixo Y
            Vector3 vertice;
            vertice.x = raio * cosA;
            vertice.y = yMundo + deslocamentoY; // usar coordenada Y transformada + incremento vertical
            vertice.z = raio * sinA;
            
            vertices.push_back(vertice);
        }
    }
    
    // gerar triângulos conectando divisões adjacentes
    int numPontosCurva = static_cast<int>(pontosCurva.size());
    for (int i = 0; i < numDivisoesRotacao; i++) {
        for (int j = 0; j < numPontosCurva - 1; j++) {
            // índices dos vértices na divisão atual
            int idx1 = i * numPontosCurva + j;
            int idx2 = i * numPontosCurva + j + 1;
            
            // conectar à próxima divisão (a última divisão conecta de volta à primeira para fechar o objeto)
            int proximaDivisao = (i + 1) % (numDivisoesRotacao + 1);
            int idx3 = proximaDivisao * numPontosCurva + j;
            int idx4 = proximaDivisao * numPontosCurva + j + 1;
            
            // criar dois triângulos para formar um quad, garantindo orientação correta das faces
            int verticesSize = static_cast<int>(vertices.size());
            if (idx1 < verticesSize && idx2 < verticesSize && idx3 < verticesSize) {
                triangulos.push_back(Triangulo(vertices[idx1], vertices[idx2], vertices[idx3]));
            }
            if (idx2 < verticesSize && idx4 < verticesSize && idx3 < verticesSize) {
                triangulos.push_back(Triangulo(vertices[idx2], vertices[idx4], vertices[idx3]));
            }
        }
    }
}

void Objeto3D::calcularNormaisVertices()
{
    if (vertices.empty() || triangulos.empty()) return;
    
    // criar acumulador de normais dos vértices para smooth shading
    std::vector<Vector3> normaisVertices(vertices.size(), Vector3(0, 0, 0));
    std::vector<int> contadorNormais(vertices.size(), 0);
    
    // encontrar índices dos vértices para cada triângulo e acumular normais
    for (int t = 0; t < static_cast<int>(triangulos.size()); t++) {
        Triangulo& tri = triangulos[t];
        
        // encontrar índices dos vértices (abordagem simples - poderia ser otimizada com tabela de lookup)
        int idx1 = -1, idx2 = -1, idx3 = -1;
        
        for (int i = 0; i < static_cast<int>(vertices.size()); i++) {
            if (vertices[i].x == tri.v1.x && vertices[i].y == tri.v1.y && vertices[i].z == tri.v1.z) {
                idx1 = i;
            }
            if (vertices[i].x == tri.v2.x && vertices[i].y == tri.v2.y && vertices[i].z == tri.v2.z) {
                idx2 = i;
            }
            if (vertices[i].x == tri.v3.x && vertices[i].y == tri.v3.y && vertices[i].z == tri.v3.z) {
                idx3 = i;
            }
        }
        
        // acumular normais para vértices encontrados
        if (idx1 >= 0) {
            normaisVertices[idx1].x += tri.normal.x;
            normaisVertices[idx1].y += tri.normal.y;
            normaisVertices[idx1].z += tri.normal.z;
            contadorNormais[idx1]++;
        }
        if (idx2 >= 0) {
            normaisVertices[idx2].x += tri.normal.x;
            normaisVertices[idx2].y += tri.normal.y;
            normaisVertices[idx2].z += tri.normal.z;
            contadorNormais[idx2]++;
        }
        if (idx3 >= 0) {
            normaisVertices[idx3].x += tri.normal.x;
            normaisVertices[idx3].y += tri.normal.y;
            normaisVertices[idx3].z += tri.normal.z;
            contadorNormais[idx3]++;
        }
    }
    
    // calcular média e normalizar normais dos vértices
    for (size_t i = 0; i < normaisVertices.size(); i++) {
        if (contadorNormais[i] > 0) {
            normaisVertices[i].x /= contadorNormais[i];
            normaisVertices[i].y /= contadorNormais[i];
            normaisVertices[i].z /= contadorNormais[i];
            normaisVertices[i].normalize();
        }
    }
    
    // atribuir normais suaves de volta aos triângulos
    for (size_t t = 0; t < triangulos.size(); t++) {
        Triangulo& tri = triangulos[t];
        
        // encontrar vértices novamente e atribuir normais suaves
        for (size_t i = 0; i < vertices.size(); i++) {
            if (vertices[i].x == tri.v1.x && vertices[i].y == tri.v1.y && vertices[i].z == tri.v1.z) {
                tri.n1 = normaisVertices[i];
            }
            if (vertices[i].x == tri.v2.x && vertices[i].y == tri.v2.y && vertices[i].z == tri.v2.z) {
                tri.n2 = normaisVertices[i];
            }
            if (vertices[i].x == tri.v3.x && vertices[i].y == tri.v3.y && vertices[i].z == tri.v3.z) {
                tri.n3 = normaisVertices[i];
            }
        }
    }
}

void Objeto3D::atualizarObjeto()
{
    gerarSweepRotacional();
    calcularNormaisVertices();
}

void Objeto3D::desenhar()
{
    if (triangulos.empty()) return;
    
    if (modoWireframe) {
        // renderização wireframe tradicional usando linhas
        CV::color(0, 0, 1); // azul para wireframe
        
        for (size_t i = 0; i < triangulos.size(); i++) {
            Triangulo& tri = triangulos[i];
            
            // projetar vértices do triângulo para 2D
            Vector2 p1 = projetarPonto(tri.v1);
            Vector2 p2 = projetarPonto(tri.v2);
            Vector2 p3 = projetarPonto(tri.v3);
            
            // desenhar triângulo em wireframe conectando as arestas
            CV::line(p1.x, p1.y, p2.x, p2.y);
            CV::line(p2.x, p2.y, p3.x, p3.y);
            CV::line(p3.x, p3.y, p1.x, p1.y);
        }
    } else {
        // nova rasterização com z-buffer e iluminação per-pixel
        if (!framebuffer || !rasterizer) return;
        
        // limpar framebuffer para nova renderização
        framebuffer->limpar();
        
        // rasterizar todos os triângulos
        for (size_t i = 0; i < triangulos.size(); i++) {
            Triangulo& tri = triangulos[i];
            
            // transformar vértices para espaço do mundo 3D
            Vector3 v1_world = transformarPonto(tri.v1);
            Vector3 v2_world = transformarPonto(tri.v2);
            Vector3 v3_world = transformarPonto(tri.v3);
            
            // transformar normais para espaço do mundo para iluminação correta
            Vector3 n1_world = transformarNormal(tri.n1);
            Vector3 n2_world = transformarNormal(tri.n2);
            Vector3 n3_world = transformarNormal(tri.n3);
            
            // projetar para espaço de tela 2D usando coordenadas do framebuffer
            Vector2 p1_2d = projetarPontoFramebuffer(tri.v1);
            Vector2 p2_2d = projetarPontoFramebuffer(tri.v2);
            Vector2 p3_2d = projetarPontoFramebuffer(tri.v3);
            
            // criar vértices de rasterização com posições 2D, posições 3D e normais do espaço do mundo
            VerticeRaster v1_raster(p1_2d, v1_world, n1_world);
            VerticeRaster v2_raster(p2_2d, v2_world, n2_world);
            VerticeRaster v3_raster(p3_2d, v3_world, n3_world);
            
            // rasterizar triângulo com iluminação
            rasterizer->rasterizarTriangulo(*framebuffer, v1_raster, v2_raster, v3_raster);
        }
        
        // renderização do framebuffer completa - o chamador cuidará de desenhar na tela
    }
}

void Objeto3D::desenharNormais()
{
    if (!mostrarNormais || triangulos.empty()) return;
    
    CV::color(1, 1, 0); // amarelo para vetores normais
    
    for (size_t i = 0; i < triangulos.size(); i++) {
        Triangulo& tri = triangulos[i];
        
        // calcular centro do triângulo para origem do vetor normal
        Vector3 centro;
        centro.x = (tri.v1.x + tri.v2.x + tri.v3.x) / 3.0f;
        centro.y = (tri.v1.y + tri.v2.y + tri.v3.y) / 3.0f;
        centro.z = (tri.v1.z + tri.v2.z + tri.v3.z) / 3.0f;
        
        // calcular ponto final da normal para visualização
        Vector3 fimNormal;
        fimNormal.x = centro.x + tri.normal.x * 20.0f;
        fimNormal.y = centro.y + tri.normal.y * 20.0f;
        fimNormal.z = centro.z + tri.normal.z * 20.0f;
        
        // projetar pontos e desenhar linha da normal
        Vector2 p1 = projetarPonto(centro);
        Vector2 p2 = projetarPonto(fimNormal);
        
        CV::line(p1.x, p1.y, p2.x, p2.y);
    }
}
