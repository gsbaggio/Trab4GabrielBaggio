@echo off
echo Testando logica das curvas de Bezier...
echo.

REM Compilar teste simples
g++ -std=c++11 -o test_bezier.exe test_bezier.cpp

if %errorlevel% neq 0 (
    echo ERRO: Falha na compilacao do teste.
    pause
    exit /b 1
)

echo Executando teste...
echo.
test_bezier.exe

echo.
echo Teste concluido. Pressione qualquer tecla para continuar...
pause >nul
