project(bce2 CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 20)

aux_source_directory(src SRC_LIST)

include_directories(
    src
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

## openssl
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})
## kyotocabinet
find_package(PkgConfig REQUIRED)
pkg_check_modules(kyotocabinet REQUIRED IMPORTED_TARGET kyotocabinet)
target_link_libraries(${PROJECT_NAME} PkgConfig::kyotocabinet)

#install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
