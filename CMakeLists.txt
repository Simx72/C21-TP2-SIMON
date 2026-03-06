cmake_minimum_required(VERSION 3.23)
project(GestionBanque)


# For editor
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compile options
# windows/linux
IF (WIN32)
    MESSAGE("Compile mode: Windows")
    # set stuff for windows
ELSE()
    MESSAGE("Compile mode: Linux")
    # set stuff for other systems
    set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
    # development/production
    IF ($ENV{DEBUG})
        # Development mode
        MESSAGE("Compile mode: Linux + Development")
        add_compile_options(-DDEBUG -O0)
    ELSE()
        # Production mode
        MESSAGE("Compile mode: Linux + Production")
        add_compile_options(-DNDEBUG -O3)
        add_compile_options(-Wall -Wextra -Wpedantic)
    ENDIF()
ENDIF()

# Link
add_link_options(
    -static-libstdc++
    -static-libgcc
)

add_executable(GestionBanque)

target_sources(GestionBanque
    PRIVATE
        main.cpp
        cvm_23.cpp
)
