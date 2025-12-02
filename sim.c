#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned long vpn;  
    int valid;          // 1: Ocupado, 0: Vacío
    int use_bit;        
} Frame;

// Calcula cuántos bits se necesitan para el offset
int get_offset_bits(int page_size) {
    int bits = 0;
    while (page_size > 1) {
        page_size >>= 1; // Desplazamiento a la derecha (dividir por 2)
        bits++;
    }
    return bits;
}

int main(int argc, char* argv[]) {
    // VALIDACIÓN DE ARGUMENTOS
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <Nmarcos> <PageSize> [--verbose] <traza.txt>\n", argv[0]);
        return 1;
    }

    int n_marcos = atoi(argv[1]);
    int page_size = atoi(argv[2]);
    char* filename = NULL;
    int verbose = 0;

    // Detectar "--verbose" y cuál es el archivo
    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else {
            filename = argv[i];
        }
    }

    if (!filename) {
        fprintf(stderr, "Error: No especificaste el archivo de traza.\n");
        return 1;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error abriendo el archivo");
        return 1;
    }

    // PREPARACIÓN DE CONSTANTES BINARIAS
    int b = get_offset_bits(page_size);     // Bits de desplazamiento (offset)
    unsigned long mask = page_size - 1;     // Máscara para extraer el offset (ej: 0xFFF)

    // Crear la "Memoria Física" 
    Frame* frames = (Frame*)malloc(sizeof(Frame) * n_marcos);
    for (int i = 0; i < n_marcos; i++) {
        frames[i].valid = 0;    
        frames[i].use_bit = 0;
        frames[i].vpn = 0;
    }

    // Variables de la simulación
    char line[256];
    unsigned long va;       // Virtual Address
    int clock_hand = 0;     // La "manecilla" del reloj
    long total_refs = 0;
    long page_faults = 0;

    // PROCESAMIENTO LÍNEA POR LÍNEA
    while (fgets(line, sizeof(line), file)) {
        // strtoul con base 0 detecta automáticamente decimal o hex (0x...)
        va = strtoul(line, NULL, 0); 
        total_refs++;

        unsigned long offset = va & mask; // Extraer los bits bajos (offset)
        unsigned long vpn = va >> b;      // Extraer los bits altos (número de página)

        int frame_found = -1; 
        int hit = 0;

        // BUSCAR EN MEMORIA 
        for (int i = 0; i < n_marcos; i++) {
            if (frames[i].valid && frames[i].vpn == vpn) {
                frame_found = i;
                hit = 1;
                frames[i].use_bit = 1; 
                break;
            }
        }

        // C. SI ES UN MISS (FALLO), EJECUTAR ALGORITMO RELOJ
        if (!hit) {
            page_faults++;
            
            while (1) {
                // Caso 1: Marco vacío -> Usarlo directo
                if (!frames[clock_hand].valid) {
                    frame_found = clock_hand;
                    clock_hand = (clock_hand + 1) % n_marcos; 
                    break;
                }

                // Caso 2: Marco ocupado -> Revisar el bit de uso
                if (frames[clock_hand].use_bit == 1) {
                    frames[clock_hand].use_bit = 0; // Perdonado (Bit 1->0)
                    clock_hand = (clock_hand + 1) % n_marcos; // Avanzar manecilla
                } else {
                    frame_found = clock_hand;
                    clock_hand = (clock_hand + 1) % n_marcos; 
                    break; 
                }
            }

            // Actualizar la tabla con la nueva página
            frames[frame_found].vpn = vpn;
            frames[frame_found].valid = 1;
            frames[frame_found].use_bit = 1; 
        }

        // D. CALCULAR DIRECCIÓN FÍSICA
        unsigned long pa = ((unsigned long)frame_found << b) | offset;

        // E. IMPRIMIR SI ES VERBOSE
        if (verbose) {
            printf("DV: 0x%lx, nvp: 0x%lx, offset: 0x%lx, %s, marco: %d, DF: 0x%lx\n",
                   va, vpn, offset, hit ? "HIT" : "FALLO", frame_found, pa);
        }
    }

    // 4. REPORTE FINAL
    double tasa = 0.0;
    if (total_refs > 0) tasa = (double)page_faults / total_refs * 100.0;

    printf("\nTotales: Referencias: %ld, Fallos de página: %ld, Tasa de fallos: %.2f%%\n",
           total_refs, page_faults, tasa);

    free(frames);
    fclose(file);
    return 0;
}