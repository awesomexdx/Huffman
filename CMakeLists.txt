cmake_minimum_required(VERSION 3.12)
project(Huffman C)

set(CMAKE_C_STANDARD 11)

add_executable(Huffman main.c definitions.h queue.c sort.c sort.h coding.c coding.h)