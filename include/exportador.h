#ifndef EXPORTADOR_H
#define EXPORTADOR_H

#include <stddef.h>  // para size_t
#include <stdint.h> // para uint64_t
// Create parent folder (if needed) and write the CSV header.
void init_csv(const char *path_csv);

// Append one row (Algoritmo,n,Keygen,Sign,Verify) to the CSV.
void exportar_resultado(const char *algoritmo,
                                   int n,
                                   int t_clave,  uint64_t ciclos_clave,
                                   int t_firma,  uint64_t ciclos_firma,
                                   int t_verif,  uint64_t ciclos_verif,
                                   const char *path_csv);

#endif // EXPORTADOR_H