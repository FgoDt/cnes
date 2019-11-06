cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_FLAGS "-g")

project(cnes)

add_executable(
    cnes
    "./deassembly.c"
	"./cnes_bus.c"
	"./cnes_6502.c"
    "./main.c"
)
