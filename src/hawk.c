#include "hawk.h"             // tu wrapper
#include "hawk_ref/hawk.h"    // la API real de HAWK
#include "exportador.h"
#include "utils.h"
#include "utils_random.h"            // obtener_tiempo_microsegundos()
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <x86intrin.h>  
// portable mkdir
#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #define MKDIR(p) mkdir((p),0755)
#endif

// RNG sencillo para keygen
static void rng_dummy(void *ctx, void *dst, size_t len) {
    uint8_t *b = dst;
    for (size_t i = 0; i < len; i++) {
        b[i] = rand() & 0xFF;
    }
}

void ejecutar_hawk(int log_n, int repeticiones, const char* path_csv) {
    // 0) Asegurarnos de que exista el directorio results
    MKDIR("results");
    for (int i = 0; i < repeticiones; i++) {
        size_t mlen = 32;  // puedes usar 32, 64, 128… según quieras
        uint8_t *msg = malloc(mlen);
        if (!msg) {
            perror("malloc mensaje");
            exit(EXIT_FAILURE);
        }
        generate_random_message(msg, mlen);
        
        // 1) calcular longitudes de buffers
        size_t sk_len   = HAWK_PRIVKEY_SIZE(log_n);
        size_t pk_len   = HAWK_PUBKEY_SIZE(log_n);
        size_t sig_len  = HAWK_SIG_SIZE(log_n);
        size_t tmp_key  = HAWK_TMPSIZE_KEYGEN(log_n);
        size_t tmp_sign = HAWK_TMPSIZE_SIGN(log_n);
        size_t tmp_vfy  = HAWK_TMPSIZE_VERIFY(log_n);

        // tamaño máximo de buffer temporal
        size_t tmp_len = tmp_key;
        if (tmp_sign > tmp_len) tmp_len = tmp_sign;
        if (tmp_vfy  > tmp_len) tmp_len = tmp_vfy;

        // 2) asignar buffers
        uint8_t *sk  = malloc(sk_len);
        uint8_t *pk  = malloc(pk_len);
        uint8_t *sg  = malloc(sig_len);
        uint8_t *tmp = malloc(tmp_len);
        if (!sk || !pk || !sg || !tmp) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        // 3) KEYGEN (μs)
        unsigned long long c0 = __rdtsc();
        uint64_t t0 = obtener_tiempo_microsegundos();
        if (!hawk_keygen((unsigned)log_n,
                         sk, pk,
                         rng_dummy, NULL,
                         tmp, tmp_key))
        {
            fprintf(stderr, "hawk_keygen() failed\n");
            exit(EXIT_FAILURE);
        }
        uint64_t t1 = obtener_tiempo_microsegundos();
        unsigned long long c1 = __rdtsc();
        int t_keygen_us = (int)(t1 - t0);
         unsigned long long ciclos_keygen = c1 - c0;
        // 4) SIGN (μs)
        shake_context sc;
        hawk_sign_start(&sc);
        shake_inject(&sc, msg, mlen);
        c0 = __rdtsc();
        t0 = obtener_tiempo_microsegundos();
        if (!hawk_sign_finish((unsigned)log_n,
                              rng_dummy, NULL,
                              sg, &sc,
                              sk, tmp, tmp_sign))
        {
            fprintf(stderr, "hawk_sign_finish() failed\n");
            exit(EXIT_FAILURE);
        }
        t1 = obtener_tiempo_microsegundos();
        c1 = __rdtsc();
        int t_sign_us = (int)(t1 - t0);
        unsigned long long ciclos_sign = c1 - c0;

        // 5) VERIFY (μs)
        hawk_verify_start(&sc);
        shake_inject(&sc, msg, mlen);
        c0 = __rdtsc();
        t0 = obtener_tiempo_microsegundos();
        if (!hawk_verify_finish((unsigned)log_n,
                                sg, sig_len,
                                &sc,
                                pk, pk_len,
                                tmp, tmp_vfy))
        {
            fprintf(stderr, "hawk_verify_finish() failed\n");
            exit(EXIT_FAILURE);
        }
        t1 = obtener_tiempo_microsegundos();
        c1 = __rdtsc();
        int t_verify_us = (int)(t1 - t0);
        unsigned long long ciclos_verify = c1 - c0;

        // 6) exportar línea CSV: Algoritmo,n,Keygen,Sign,Verify
        exportar_resultado(
            "HAWK",
            1 << log_n,
            t_keygen_us, ciclos_keygen,
            t_sign_us, ciclos_sign,
            t_verify_us, ciclos_verify,
            path_csv
        );

        // 7) liberar memoria
        free(sk);
        free(pk);
        free(sg);
        free(tmp);
    }
}