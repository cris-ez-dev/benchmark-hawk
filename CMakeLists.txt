cmake_minimum_required(VERSION 3.14)
project(implementacion_postcuanticos C)

# Fuerza C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Añadimos liboqs (requiere que liboqs/ esté en la raíz)
add_subdirectory(liboqs)

# Directorios de include:
# 1) Tus headers “deluxe” en include/
# 2) El reference-impl de HAWK en src/hawk_ref/
# 3) Los headers generados por liboqs
include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src/hawk_ref
  ${PROJECT_SOURCE_DIR}/src/hawk_ref_tests
  ${PROJECT_BINARY_DIR}/liboqs/include
)

# ----------------------------------------------------
# 1) Fuentes de la “parte principal” de tu programa
set(MAIN_SRCS
  src/main.c
  src/hawk.c
  src/exportador.c
  src/utils.c
  src/mldsa_oqs.c
  src/sphincs_oqs.c
  src/utils_random.c
)

# 2) Fuentes del reference‐impl de HAWK (bajo src/hawk_ref/)
set(HAWK_REF_SRCS
  src/hawk_ref/sha3.c
  src/hawk_ref/hawk_kgen.c
  src/hawk_ref/hawk_sign.c
  src/hawk_ref/hawk_vrfy.c
  src/hawk_ref/ng_fxp.c
  src/hawk_ref/ng_hawk.c
  src/hawk_ref/ng_poly.c
  src/hawk_ref/ng_mp31.c
  src/hawk_ref/ng_ntru.c
  src/hawk_ref/ng_zint31.c
)

# Construimos el ejecutable a partir de TODO
add_executable(main
  ${MAIN_SRCS}
  ${HAWK_REF_SRCS}
)

# Enlazamos contra liboqs (y, si te hace falta math.h, puedes añadir 'm')
target_link_libraries(main PRIVATE oqs)
# target_link_libraries(main PRIVATE oqs m)