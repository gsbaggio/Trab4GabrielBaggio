@echo off
echo Compilando Aplicacao de Modelagem 3D com Curvas de Bezier...
echo.

REM Verificar se o compilador está disponível
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo ERRO: g++ nao encontrado. Instale MinGW ou configure o PATH.
    pause
    exit /b 1
)

REM Criar diretório de build se não existir
if not exist build mkdir build

REM Compilar
echo Compilando arquivos fonte...
g++ -std=c++11 -Wall -O2 -c src/main.cpp -o build/main.o
g++ -std=c++11 -Wall -O2 -c src/Aplicacao3D.cpp -o build/Aplicacao3D.o
g++ -std=c++11 -Wall -O2 -c src/CurvaBezier.cpp -o build/CurvaBezier.o
g++ -std=c++11 -Wall -O2 -c src/Objeto3D.cpp -o build/Objeto3D.o
g++ -std=c++11 -Wall -O2 -c src/gl_canvas2d.cpp -o build/gl_canvas2d.o

REM Verificar se a compilação foi bem-sucedida
if %errorlevel% neq 0 (
    echo ERRO: Falha na compilacao.
    pause
    exit /b 1
)

REM Linkar
echo Linkando executavel...
g++ build/*.o -o bezier3d.exe -lopengl32 -lglu32 -lfreeglut

REM Verificar se a linkagem foi bem-sucedida
if %errorlevel% neq 0 (
    echo ERRO: Falha na linkagem. Verifique se as bibliotecas OpenGL/GLUT estao instaladas.
    pause
    exit /b 1
)

echo.
echo Compilacao concluida com sucesso!
echo Executavel criado: bezier3d.exe
echo.
echo Para executar, digite: bezier3d.exe
echo.
pause
