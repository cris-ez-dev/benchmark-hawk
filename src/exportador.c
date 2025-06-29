#include "exportador.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// portable mkdir
#ifdef _WIN32
  #include <direct.h>
  #define MKDIR(path) _mkdir(path)
#else
  #include <sys/stat.h>
  #define MKDIR(path) mkdir((path), 0755)
#endif

void init_csv(const char *path_csv) {
    // Extract directory component
    char folder[256];
    strncpy(folder, path_csv, sizeof(folder));
    folder[sizeof(folder)-1] = '\0';
    char *sep = strrchr(folder, '/');
    if (!sep) {
        sep = strrchr(folder, '\\');
    }
    if (sep) {
        *sep = '\0';
        // attempt to create the folder (ignore errors)
        MKDIR(folder);
    }

    // Open for overwrite and write header
    FILE *f = fopen(path_csv, "w");
    if (!f) {
        perror("init_csv");
        return;
    }
    fprintf(f, "Algoritmo,n,"
                "Keygen_us,Keygen_ciclos,"
                "Sign_us,Sign_ciclos,"
                "Verify_us,Verify_ciclos\n");
    fclose(f);
}

void exportar_resultado(const char *algoritmo,
                                   int n,
                                   int t_clave,  uint64_t ciclos_clave,
                                   int t_firma,  uint64_t ciclos_firma,
                                   int t_verif,  uint64_t ciclos_verif,
                                   const char *path_csv)
{
    FILE *f = fopen(path_csv, "a");
    if (!f) {
        perror("exportar_resultado");
        return;
    }
    fprintf(f, "%s,%d,%d,%llu,%d,%llu,%d,%llu\n",
            algoritmo,
            n,
            t_clave,  (unsigned long long)ciclos_clave,
            t_firma,  (unsigned long long)ciclos_firma,
            t_verif,  (unsigned long long)ciclos_verif);
    fclose(f);
}