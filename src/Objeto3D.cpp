#include "Objeto3D.h"
#include "gl_canvas2d.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265359
#endif

Objeto3D::Objeto3D(CurvaBezier* curva)
{
    curvaBezier = curva;
    numDivisoesRotacao = 10; // Ajustado para 10 divisões iniciais
    sweepTranslacional = 0.0f; // Sem incremento vertical inicial
    mostrarNormais = false;
    modoWireframe = true;
    projecaoPerspectiva = false; // Já está em ortográfica
    distanciaCamera = 800.0f; // Aumentado de 300 para 800 - câmera mais distante
    
    // Initialize rendering system
    framebuffer = nullptr;
    rasterizer = new Rasterizer();    // Setup default lighting - light to the left and behind
    rasterizer->definirLuz(Vector3(-0.5f, 0.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f));
    rasterizer->definirCorAmbiente(Vector3(0.2f, 0.2f, 0.2f));
    rasterizer->definirCorMaterial(Vector3(0.4f, 0.6f, 0.8f)); // Blue material
    
    // Inicializar transformações
    rotacaoX = rotacaoY = rotacaoZ = 0.0f;
    translacaoX = translacaoY = translacaoZ = 0.0f;
    escala = 0.6f; // Aumentado de 0.3 para 0.6 - objeto com mais zoom inicial
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
    escala = 0.6f; // Mesma escala inicial aumentada
    distanciaCamera = 800.0f; // Mesma distância inicial
}

void Objeto3D::definirDistanciaCamera(float distancia)
{
    distanciaCamera = distancia;
}

Vector3 Objeto3D::transformarPonto(Vector3 ponto)
{
    Vector3 p = ponto;
    
    // Aplicar escala
    p = p * escala;
    
    // Aplicar rotações
    // Rotação em X
    if (rotacaoX != 0) {
        float cosX = cos(rotacaoX);
        float sinX = sin(rotacaoX);
        float y = p.y * cosX - p.z * sinX;
        float z = p.y * sinX + p.z * cosX;
        p.y = y;
        p.z = z;
    }
    
    // Rotação em Y
    if (rotacaoY != 0) {
        float cosY = cos(rotacaoY);
        float sinY = sin(rotacaoY);
        float x = p.x * cosY + p.z * sinY;
        float z = -p.x * sinY + p.z * cosY;
        p.x = x;
        p.z = z;
    }
    
    // Rotação em Z
    if (rotacaoZ != 0) {
        float cosZ = cos(rotacaoZ);
        float sinZ = sin(rotacaoZ);
        float x = p.x * cosZ - p.y * sinZ;
        float y = p.x * sinZ + p.y * cosZ;
        p.x = x;
        p.y = y;
    }
    
    // Aplicar translação
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
        // Projeção perspectiva
        float fator = distanciaCamera / (distanciaCamera + p.z);
        resultado.x = p.x * fator;
        resultado.y = p.y * fator;
    } else {
        // Projeção ortográfica
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
        // Projeção perspectiva
        float fator = distanciaCamera / (distanciaCamera + p.z);
        resultado.x = p.x * fator;
        resultado.y = p.y * fator;
    } else {
        // Projeção ortográfica
        resultado.x = p.x;
        resultado.y = p.y;
    }
      // Convert from OpenGL coordinates to framebuffer pixel coordinates
    if (framebuffer) {
        // Center the object in the framebuffer and keep same Y orientation as wireframe
        resultado.x = (framebuffer->getLargura() / 2) + resultado.x;
        resultado.y = (framebuffer->getAltura() / 2) + resultado.y; // Keep same Y direction as wireframe
    }
    
    return resultado;
}

Vector3 Objeto3D::transformarNormal(Vector3 normal)
{
    Vector3 n = normal;
    
    // Apply only rotations to normals (no scale or translation)
    // Rotation in X
    if (rotacaoX != 0) {
        float cosX = cos(rotacaoX);
        float sinX = sin(rotacaoX);
        float y = n.y * cosX - n.z * sinX;
        float z = n.y * sinX + n.z * cosX;
        n.y = y;
        n.z = z;
    }
    
    // Rotation in Y
    if (rotacaoY != 0) {
        float cosY = cos(rotacaoY);
        float sinY = sin(rotacaoY);
        float x = n.x * cosY + n.z * sinY;
        float z = -n.x * sinY + n.z * cosY;
        n.x = x;
        n.z = z;
    }
    
    // Rotation in Z
    if (rotacaoZ != 0) {
        float cosZ = cos(rotacaoZ);
        float sinZ = sin(rotacaoZ);
        float x = n.x * cosZ - n.y * sinZ;
        float y = n.x * sinZ + n.y * cosZ;
        n.x = x;
        n.y = y;
    }
    
    // Normalize the normal after transformation
    n.normalize();
    
    return n;
}

void Objeto3D::gerarSweepRotacional()
{
    vertices.clear();
    triangulos.clear();
    
    std::vector<Vector2>& pontosCurva = curvaBezier->getPontosCurva();
    if (pontosCurva.size() < 2) return;
      // Gerar vértices através de rotação
    float anguloIncremento = 2.0f * PI / numDivisoesRotacao;
      for (int i = 0; i <= numDivisoesRotacao; i++) {
        float angulo = i * anguloIncremento;
        float cosA = cos(angulo);
        float sinA = sin(angulo);
          // Calcular deslocamento vertical para sweep translacional
        // Valores positivos fazem crescer para cima, negativos para baixo
        float deslocamentoY = -i * sweepTranslacional;
        
        for (int j = 0; j < pontosCurva.size(); j++) {
            Vector2& p = pontosCurva[j];
            float raio = fabs(p.x); // Usar fabs ao invés de abs para float
            
            Vector3 vertice;
            vertice.x = raio * cosA;
            vertice.y = p.y + deslocamentoY; // Adicionar incremento vertical
            vertice.z = raio * sinA;
            
            vertices.push_back(vertice);
        }
    }
      // Gerar triângulos
    int numPontosCurva = static_cast<int>(pontosCurva.size());
    for (int i = 0; i < numDivisoesRotacao; i++) {
        for (int j = 0; j < numPontosCurva - 1; j++) {
            int idx1 = i * numPontosCurva + j;
            int idx2 = i * numPontosCurva + j + 1;
            int idx3 = ((i + 1) % (numDivisoesRotacao + 1)) * numPontosCurva + j;
            int idx4 = ((i + 1) % (numDivisoesRotacao + 1)) * numPontosCurva + j + 1;
            
            // Criar dois triângulos para formar um quad
            int verticesSize = static_cast<int>(vertices.size());
            if (idx1 < verticesSize && idx2 < verticesSize && idx3 < verticesSize) {
                triangulos.push_back(Triangulo(vertices[idx1], vertices[idx2], vertices[idx3]));
            }
            if (idx2 < verticesSize && idx3 < verticesSize && idx4 < verticesSize) {
                triangulos.push_back(Triangulo(vertices[idx2], vertices[idx4], vertices[idx3]));
            }
        }
    }
}

void Objeto3D::calcularNormaisVertices()
{
    if (vertices.empty() || triangulos.empty()) return;
    
    // Create vertex normal accumulator
    std::vector<Vector3> normaisVertices(vertices.size(), Vector3(0, 0, 0));
    std::vector<int> contadorNormais(vertices.size(), 0);
      // Find vertex indices for each triangle and accumulate normals
    for (int t = 0; t < static_cast<int>(triangulos.size()); t++) {
        Triangulo& tri = triangulos[t];
        
        // Find vertex indices (simple approach - could be optimized with a lookup table)
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
        
        // Accumulate normals for found vertices
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
    
    // Average and normalize vertex normals
    for (int i = 0; i < normaisVertices.size(); i++) {
        if (contadorNormais[i] > 0) {
            normaisVertices[i].x /= contadorNormais[i];
            normaisVertices[i].y /= contadorNormais[i];
            normaisVertices[i].z /= contadorNormais[i];
            normaisVertices[i].normalize();
        }
    }
    
    // Assign smooth normals back to triangles
    for (int t = 0; t < triangulos.size(); t++) {
        Triangulo& tri = triangulos[t];
        
        // Find vertices again and assign smooth normals
        for (int i = 0; i < vertices.size(); i++) {
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
        // Traditional wireframe rendering
        CV::color(0, 0, 1); // Azul para wireframe
        
        for (int i = 0; i < triangulos.size(); i++) {
            Triangulo& tri = triangulos[i];
            
            Vector2 p1 = projetarPonto(tri.v1);
            Vector2 p2 = projetarPonto(tri.v2);
            Vector2 p3 = projetarPonto(tri.v3);
            
            // Desenhar triângulo em wireframe
            CV::line(p1.x, p1.y, p2.x, p2.y);
            CV::line(p2.x, p2.y, p3.x, p3.y);
            CV::line(p3.x, p3.y, p1.x, p1.y);
        }
    } else {
        // New rasterization with z-buffer and per-pixel lighting
        if (!framebuffer || !rasterizer) return;
        
        // Clear framebuffer
        framebuffer->limpar();
        
        // Rasterize all triangles
        for (int i = 0; i < triangulos.size(); i++) {
            Triangulo& tri = triangulos[i];
              // Transform vertices to 3D world space
            Vector3 v1_world = transformarPonto(tri.v1);
            Vector3 v2_world = transformarPonto(tri.v2);
            Vector3 v3_world = transformarPonto(tri.v3);
            
            // Transform normals to world space for proper lighting
            Vector3 n1_world = transformarNormal(tri.n1);
            Vector3 n2_world = transformarNormal(tri.n2);
            Vector3 n3_world = transformarNormal(tri.n3);
              // Project to 2D screen space using framebuffer coordinates
            Vector2 p1_2d = projetarPontoFramebuffer(tri.v1);
            Vector2 p2_2d = projetarPontoFramebuffer(tri.v2);
            Vector2 p3_2d = projetarPontoFramebuffer(tri.v3);
            
            // Create raster vertices with 2D positions, 3D positions, and world-space normals
            VerticeRaster v1_raster(p1_2d, v1_world, n1_world);
            VerticeRaster v2_raster(p2_2d, v2_world, n2_world);
            VerticeRaster v3_raster(p3_2d, v3_world, n3_world);
              // Rasterize triangle with lighting
            rasterizer->rasterizarTriangulo(*framebuffer, v1_raster, v2_raster, v3_raster);        }
        
        // Framebuffer rendering complete - the caller will handle drawing it to screen
    }
}

void Objeto3D::desenharNormais()
{
    if (!mostrarNormais || triangulos.empty()) return;
    
    CV::color(1, 1, 0); // Amarelo para normais
    
    for (int i = 0; i < triangulos.size(); i++) {
        Triangulo& tri = triangulos[i];
        
        // Calcular centro do triângulo
        Vector3 centro;
        centro.x = (tri.v1.x + tri.v2.x + tri.v3.x) / 3.0f;
        centro.y = (tri.v1.y + tri.v2.y + tri.v3.y) / 3.0f;
        centro.z = (tri.v1.z + tri.v2.z + tri.v3.z) / 3.0f;
        
        // Ponto final da normal
        Vector3 fimNormal;
        fimNormal.x = centro.x + tri.normal.x * 20.0f;
        fimNormal.y = centro.y + tri.normal.y * 20.0f;
        fimNormal.z = centro.z + tri.normal.z * 20.0f;
        
        Vector2 p1 = projetarPonto(centro);
        Vector2 p2 = projetarPonto(fimNormal);
        
        CV::line(p1.x, p1.y, p2.x, p2.y);
    }
}
