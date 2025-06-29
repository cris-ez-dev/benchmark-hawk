// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/**
 * Inicializa el temporizador de alta resolución (solo en Windows;
 * en POSIX queda como no‐op).
 */
void timer_init(void);

/**
 * Devuelve el tiempo transcurrido (monótono) en microsegundos.
 */
uint64_t obtener_tiempo_microsegundos(void);

#endif // UTILS_H