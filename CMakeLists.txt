cmake_minimum_required(VERSION 3.5)

project(bce2 LANGUAGES CXX)
project(bce2 VERSION 0.0.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include include/kv)
aux_source_directory(src SRC_LIST)
aux_source_directory(src/load SRC_LIST)
aux_source_directory(src/parse SRC_LIST)
aux_source_directory(src/kv SRC_LIST)
aux_source_directory(src/out SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
add_executable(btcbklocs tools/btcbklocs.cpp)
add_executable(btclocview tools/btclocview.cpp)
# add_executable(test_cin tools/test_cin.cpp)

## openssl
if (APPLE)
    # brew hack
    set (OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif (APPLE)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})

# Define k-v backend
find_package(PkgConfig REQUIRED)
## kyotocabinet
pkg_check_modules(kyotocabinet REQUIRED IMPORTED_TARGET kyotocabinet)
target_link_libraries(${PROJECT_NAME} PkgConfig::kyotocabinet)
## tkrzw
option(USE_TK "Tkrzw" OFF)
if (USE_TK)
  pkg_search_module(TK IMPORTED_TARGET tkrzw)
  if (TK_FOUND)
    add_definitions(-DUSE_TK)
    target_link_libraries(${PROJECT_NAME} PkgConfig::TK)
  else ()
    set(USE_TK OFF)
  endif ()
endif ()
message(STATUS "Tkrzw: ${USE_TK}")

## leveldb
if (APPLE)
    # brew hack
    include_directories(/usr/local/opt/leveldb/include/)
    target_link_libraries(btcbklocs /usr/local/opt/leveldb/lib/libleveldb.dylib)
else(APPLE)
    find_package(leveldb CONFIG REQUIRED)
    # include_directories(${LEVELDB_INCLUDE_DIR})
    # target_link_libraries(${PROJECT_NAME} ${LEVELDB_LIBRARY})
    target_link_libraries(btcbklocs leveldb)
endif(APPLE)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(TARGETS btcbklocs RUNTIME DESTINATION bin)
install(TARGETS btclocview RUNTIME DESTINATION bin)
# install(TARGETS test_cin RUNTIME DESTINATION bin)
