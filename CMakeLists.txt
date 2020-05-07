project(bce CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 17)

aux_source_directory(src SRC_LIST)

include_directories(
    src
)

add_executable(${PROJECT_NAME} ${SRC_LIST})

## openssl
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})

#install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
