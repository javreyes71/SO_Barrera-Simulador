
###Parte 1: Barrera

#compilar
make
#Prueba por defecto (5h,4e)
./test_barrera
#Prueba con parametros (ej: 10h, 3e)
./test_barrera 10 3




###Parte 2: Simulador

#compilar
gcc -Wall -o sim sim.c
#Ejecución en particular (ej: 4 marcos, 4096 bytes)
./sim 4 4096 traces/trace.txt
#Experimentos
python3 analisis.py



### Javier Reyes G, Sistemas Operativos, Udec 2025. 
