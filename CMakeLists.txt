cmake_minimum_required(VERSION 3.10)
project(rpg_editor)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES 
    main.cpp
    npc.cpp
    knight.cpp
    elf.cpp
    dragon.cpp
    battleManager.cpp
)

set(HEADERS
    npc.h
    knight.h
    elf.h
    dragon.h
    battleManager.h
)

add_executable(rpg_editor ${SOURCES} ${HEADERS})