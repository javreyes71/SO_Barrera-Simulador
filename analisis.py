import sys
import os

CARPETA_TRACES = "traces"

def simular_reloj(nombre_archivo, num_marcos, tam_pagina):
    ruta_archivo = os.path.join(CARPETA_TRACES, nombre_archivo)
    shift_bits = tam_pagina.bit_length() - 1
    
    marcos = [{'vpn': -1, 'valid': 0, 'use': 0} for _ in range(num_marcos)]
    clock_hand = 0
    page_faults = 0
    total_refs = 0

    try:
        with open(ruta_archivo, 'r') as f:
            for linea in f:
                linea = linea.strip()
                if not linea: continue
                direc_virtual = int(linea, 0) 
                total_refs += 1
                vpn = direc_virtual >> shift_bits
                
                hit = False
                for marco in marcos:
                    if marco['valid'] and marco['vpn'] == vpn:
                        hit = True
                        marco['use'] = 1 
                        break
                
                if not hit:
                    page_faults += 1
                    while True:
                        current_frame = marcos[clock_hand]
                        
                        # Si encontramos  o víctima (bit=0)
                        if not current_frame['valid'] or current_frame['use'] == 0:
                            current_frame['vpn'] = vpn
                            current_frame['valid'] = 1
                            current_frame['use'] = 1
                            clock_hand = (clock_hand + 1) % num_marcos
                            break
                        else:
                            # Segunda oportunidad (bit 1 -> 0)
                            current_frame['use'] = 0
                            clock_hand = (clock_hand + 1) % num_marcos

        tasa_fallos = (page_faults / total_refs * 100) if total_refs > 0 else 0
        return total_refs, page_faults, tasa_fallos

    except FileNotFoundError:
        print(f"ERROR: No se encontró el archivo en: {ruta_archivo}")
        return 0, 0, 0

# --- EJECUCIÓN DE EXPERIMENTOS ---
print(f"{'Archivo':<12} | {'PgSize':<8} | {'Marcos':<6} | {'Refs':<8} | {'Fallos':<8} | {'Tasa (%)':<8}")
print("-" * 65)

# Experimento 1: trace1.txt con páginas de 8 bytes
configs_1 = [(8, 8), (16, 8), (32, 8)] 
for marcos, pg_size in configs_1:
    refs, fallos, tasa = simular_reloj('trace1.txt', marcos, pg_size)
    print(f"{'trace1.txt':<12} | {pg_size:<8} | {marcos:<6} | {refs:<8} | {fallos:<8} | {tasa:.2f}%")

print("-" * 65)

# Experimento 2: trace2.txt con páginas de 4096 bytes
configs_2 = [(8, 4096), (16, 4096), (32, 4096)]
for marcos, pg_size in configs_2:
    refs, fallos, tasa = simular_reloj('trace2.txt', marcos, pg_size)
    print(f"{'trace2.txt':<12} | {pg_size:<8} | {marcos:<6} | {refs:<8} | {fallos:<8} | {tasa:.2f}%")