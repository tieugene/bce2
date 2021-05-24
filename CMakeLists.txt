cmake_minimum_required(VERSION 3.5)

project(bce2 LANGUAGES CXX)
project(bce2 VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
aux_source_directory(src SRC_LIST)
aux_source_directory(src/bk SRC_LIST)
aux_source_directory(src/cfg SRC_LIST)
aux_source_directory(src/crypt SRC_LIST)
aux_source_directory(src/kv SRC_LIST)
aux_source_directory(src/load SRC_LIST)
aux_source_directory(src/out SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
# add_executable(test_cin tools/test_cin.cpp)

find_package(PkgConfig REQUIRED)

# for debug
option(ADDR_ONLY "Addr only" OFF)
if (ADDR_ONLY)
  add_definitions(-DADDR_ONLY)
endif()
message(STATUS "Addr only: ${ADDR_ONLY}")

## openssl
if (APPLE)
    # brew hack
    set (OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif (APPLE)
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY})

# Define k-v backend

## Kyotocabinet
option(USE_KC "Kyotocabinet" OFF)
if (USE_KC)
  pkg_search_module(KC IMPORTED_TARGET kyotocabinet)
  if (KC_FOUND)
    add_definitions(-DUSE_KC)
    target_link_libraries(${PROJECT_NAME} PkgConfig::KC)
  else ()
    set(USE_KC OFF)
  endif ()
endif ()
message(STATUS "Kyotocabinet: ${USE_KC}")

## Tkrzw
option(USE_TK "Tkrzw" ON)
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

## BerkeleyDB
option(USE_BDB "BerkeleyDB" OFF)
if (USE_BDB)
  if (APPLE)
    find_library(BDB_LIBRARY db_cxx /usr/local/opt/berkeley-db@4/lib)
  else ()
    find_library(BDB_LIBRARY db_cxx)
  endif ()
  if (BDB_LIBRARY)
      add_definitions(-DUSE_BDB)
      if (APPLE)
        include_directories(/usr/local/opt/berkeley-db/include/)
        target_link_libraries(${PROJECT_NAME} /usr/local/opt/berkeley-db/lib/libdb_cxx.dylib)
      else ()
        target_link_libraries(${PROJECT_NAME} db_cxx)
      endif ()
  else ()
    set(USE_BDB OFF)
  endif ()
endif ()
message(STATUS "BDB: ${USE_BDB}")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# ====
option(BUILD_TOOLS "Tools" OFF)
if (BUILD_TOOLS)
  add_executable(btcbklocs tools/btcbklocs.cpp)
  add_executable(btclocview tools/btclocview.cpp)
  # for btcbklocs
  ## leveldb
  if (APPLE)
      # brew hack
      include_directories(/usr/local/opt/leveldb/include/)
      target_link_libraries(btcbklocs /usr/local/opt/leveldb/lib/libleveldb.dylib)
  else ()
      find_package(leveldb CONFIG REQUIRED)
      target_link_libraries(btcbklocs leveldb)
  endif ()
  install(TARGETS btcbklocs RUNTIME DESTINATION bin)
  install(TARGETS btclocview RUNTIME DESTINATION bin)
endif ()

# install(TARGETS test_cin RUNTIME DESTINATION bin)
