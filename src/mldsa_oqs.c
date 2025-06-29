#include "mldsa_oqs.h"
#include "exportador.h"
#include "utils.h"          // ¡importante!
#include "configuracion.h"
#include "utils_random.h"

#include <oqs/oqs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

void ejecutar_mldsa(const char* alg_id, int n, int repeticiones, const char* path_csv) {
    OQS_SIG *sig = OQS_SIG_new(alg_id);
    if (sig == NULL) {
        fprintf(stderr, "Error al inicializar %s\n", alg_id);
        return;
    }

    for (int i = 0; i < repeticiones; i++) {
        uint8_t *public_key  = malloc(sig->length_public_key);
        uint8_t *secret_key  = malloc(sig->length_secret_key);
        uint8_t *signature   = malloc(sig->length_signature);
        size_t  sig_len;

        if (!public_key || !secret_key || !signature) {
            perror("malloc");
            OQS_SIG_free(sig);
            return;
        }

        size_t mensaje_len = 32;
        uint8_t *mensaje = malloc(mensaje_len);
        if (!mensaje) {
            perror("malloc mensaje");
            OQS_SIG_free(sig);
            return;
        }
        generate_random_message(mensaje, mensaje_len);

        // Keygen (μs)
        unsigned long long c0 = __rdtsc();
        uint64_t t0 = obtener_tiempo_microsegundos();
        OQS_SIG_keypair(sig, public_key, secret_key);
        uint64_t t1 = obtener_tiempo_microsegundos();
        unsigned long long c1 = __rdtsc();
        int t_keygen_us = (int)(t1 - t0);
        unsigned long long ciclos_keygen = c1 - c0;

        // Sign (μs)
        c0 = __rdtsc();
        t0 = obtener_tiempo_microsegundos();
        OQS_SIG_sign(sig, signature, &sig_len, mensaje, mensaje_len, secret_key);
        t1 = obtener_tiempo_microsegundos();
        c1 = __rdtsc();
        int t_sign_us = (int)(t1 - t0);
        unsigned long long ciclos_sign = c1 - c0;

        // Verify (μs)
        c0 = __rdtsc();
        t0 = obtener_tiempo_microsegundos();
        OQS_SIG_verify(sig, mensaje, mensaje_len, signature, sig_len, public_key);
        t1 = obtener_tiempo_microsegundos();
        c1 = __rdtsc();
        int t_verify_us = (int)(t1 - t0);
        unsigned long long ciclos_verify = c1 - c0;

        // Exportar: Algoritmo,n,Keygen,Sign,Verify
        exportar_resultado(
            alg_id,
            n,
            t_keygen_us, ciclos_keygen,
            t_sign_us, ciclos_sign,
            t_verify_us, ciclos_verify,
            path_csv
        );

        free(public_key);
        free(secret_key);
        free(signature);
    }

    OQS_SIG_free(sig);
}