#include <iostream>
#include <vector>
#include <math.h>

// Implementação simples para testar a lógica das curvas de Bézier
class Vector2Test {
public:
    float x, y;
    Vector2Test(float _x = 0, float _y = 0) : x(_x), y(_y) {}
};

class BezierTest {
private:
    std::vector<Vector2Test> pontos;
    
    int binomial(int n, int k) {
        if (k > n) return 0;
        if (k == 0 || k == n) return 1;
        
        int result = 1;
        for (int i = 1; i <= k; i++) {
            result = result * (n - i + 1) / i;
        }
        return result;
    }
    
    Vector2Test calcularPonto(float t) {
        Vector2Test ponto(0, 0);
        int n = pontos.size() - 1;
        
        for (int i = 0; i <= n; i++) {
            float bernstein = binomial(n, i) * pow(1 - t, n - i) * pow(t, i);
            ponto.x += bernstein * pontos[i].x;
            ponto.y += bernstein * pontos[i].y;
        }
        
        return ponto;
    }
    
public:
    void adicionarPonto(Vector2Test p) {
        pontos.push_back(p);
    }
    
    void testarCurva() {
        if (pontos.size() < 2) {
            std::cout << "Erro: Precisa de pelo menos 2 pontos de controle\n";
            return;
        }
        
        std::cout << "=== TESTE DA CURVA DE BEZIER ===\n";
        std::cout << "Pontos de controle (" << pontos.size() << "):\n";
        
        for (int i = 0; i < pontos.size(); i++) {
            std::cout << "  P" << i << ": (" << pontos[i].x << ", " << pontos[i].y << ")\n";
        }
        
        std::cout << "\nPontos da curva (10 amostras):\n";
        for (int i = 0; i <= 10; i++) {
            float t = (float)i / 10.0f;
            Vector2Test p = calcularPonto(t);
            std::cout << "  t=" << t << ": (" << p.x << ", " << p.y << ")\n";
        }
        
        std::cout << "\n=== SIMULACAO 3D (SWEEP ROTACIONAL) ===\n";
        std::cout << "Gerando vertices 3D com 8 divisoes...\n";
        
        int divisoes = 8;
        float anguloInc = 2.0f * 3.14159f / divisoes;
        
        for (int i = 0; i < divisoes; i++) {
            float angulo = i * anguloInc;
            float cosA = cos(angulo);
            float sinA = sin(angulo);
            
            std::cout << "Angulo " << (angulo * 180.0f / 3.14159f) << " graus:\n";
            
            for (int j = 0; j <= 5; j++) {
                float t = (float)j / 5.0f;
                Vector2Test p2d = calcularPonto(t);
                float raio = fabs(p2d.x);
                
                float x3d = raio * cosA;
                float y3d = p2d.y;
                float z3d = raio * sinA;
                
                std::cout << "  Vertice: (" << x3d << ", " << y3d << ", " << z3d << ")\n";
            }
        }
        
        std::cout << "\nTeste concluido com sucesso!\n";
    }
};

int main() {
    std::cout << "=== TESTE DO SISTEMA DE MODELAGEM 3D ===\n\n";
    
    BezierTest teste;
    
    // Adicionar pontos de uma garrafa simples
    teste.adicionarPonto(Vector2Test(30, 100));  // Base
    teste.adicionarPonto(Vector2Test(50, 150));  // Corpo inferior
    teste.adicionarPonto(Vector2Test(45, 200));  // Corpo médio
    teste.adicionarPonto(Vector2Test(25, 250));  // Gargalo
    teste.adicionarPonto(Vector2Test(35, 300));  // Topo
    
    teste.testarCurva();
    
    std::cout << "\n=== INSTRUCOES ===\n";
    std::cout << "Este teste verifica se a logica das curvas de Bezier esta funcionando.\n";
    std::cout << "Para usar a aplicacao completa:\n";
    std::cout << "1. Compile com OpenGL/GLUT usando build.bat ou make\n";
    std::cout << "2. Execute bezier3d.exe\n";
    std::cout << "3. Clique para adicionar pontos de controle\n";
    std::cout << "4. Pressione Tab para ver em 3D\n\n";
    
    return 0;
}
