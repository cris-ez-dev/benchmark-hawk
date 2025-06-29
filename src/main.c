#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"          // timer_init(), obtener_tiempo_microsegundos()
#include "exportador.h"     // init_csv(), exportar_resultado()
#include "mldsa_oqs.h"
#include "sphincs_oqs.h"
#include "hawk.h"

int main(void) {
    // 0) Inicializaciones
    timer_init();                        // en Windows pone a punto QPC; en POSIX es no-op
    srand((unsigned)time(NULL));         // para rng_dummy()

    const char *output       = "results/resultados.csv";
    int         log_n        = 9;      // 2^10 = 1024
    int         repeticiones = 3000;

    // 1) Prepara el CSV: crea carpeta + escribe cabecera
    init_csv(output);

    // 2) Ejecuta cada esquema
    printf("Ejecutando ML-DSA-44 (n = %d, reps = %d)...\n", 1<<log_n, repeticiones);
    ejecutar_mldsa("ML-DSA-44", 1 << log_n, repeticiones, output);

    printf("Ejecutando SPHINCS+-SHAKE-128f-simple (n = %d, reps = %d)...\n", 1<<log_n, repeticiones);
    ejecutar_sphincs("SPHINCS+-SHAKE-128f-simple", 1 << log_n, repeticiones, output);

    printf("Ejecutando HAWK (log_n = %d, reps = %d)...\n", log_n, repeticiones);
    ejecutar_hawk(log_n, repeticiones, output);

    printf("Resultados guardados en %s\n", output);
    return 0;
}