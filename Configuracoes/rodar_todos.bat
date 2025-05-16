@echo off
set TRACE=traceDijkstra.txt

echo Rodando config_base.txt
simbasica.exe config_base.txt %TRACE% > resultado_base.txt

echo Rodando config_128_1_1.txt
simbasica.exe config_128_1_1.txt %TRACE% > resultado_128_1_1.txt

echo Rodando config_256_1_1.txt
simbasica.exe config_256_1_1.txt %TRACE% > resultado_256_1_1.txt

echo Rodando config_64_2_1.txt
simbasica.exe config_64_2_1.txt %TRACE% > resultado_64_2_1.txt

echo Rodando config_64_4_1.txt
simbasica.exe config_64_4_1.txt %TRACE% > resultado_64_4_1.txt

echo Rodando config_64_1_2.txt
simbasica.exe config_64_1_2.txt %TRACE% > resultado_64_1_2.txt

echo Rodando config_64_1_4.txt
simbasica.exe config_64_1_4.txt %TRACE% > resultado_64_1_4.txt

echo Todos os testes foram executados!
pause