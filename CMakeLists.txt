cmake_minimum_required(VERSION 3.20)
project(ImuxOS LANGUAGES CXX ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(imux_kernel kernel/boot.S kernel/kernel.cpp)
target_compile_options(imux_kernel PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pie -fno-pic -mno-red-zone -mcmodel=kernel -Wall -Wextra>
  $<$<COMPILE_LANGUAGE:ASM>:-m64>)
target_link_options(imux_kernel PRIVATE -nostdlib -static -no-pie -Wl,--build-id=none -Wl,-T,${CMAKE_SOURCE_DIR}/kernel/linker.ld)
set_target_properties(imux_kernel PROPERTIES OUTPUT_NAME "kernel.elf")
add_custom_target(iso ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/iso/boot/grub
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:imux_kernel> ${CMAKE_BINARY_DIR}/iso/boot/kernel.elf
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/grub/grub.cfg ${CMAKE_BINARY_DIR}/iso/boot/grub/grub.cfg
  COMMAND grub-file --is-x86-multiboot2 ${CMAKE_BINARY_DIR}/iso/boot/kernel.elf
  COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/ImuxOS.iso ${CMAKE_BINARY_DIR}/iso
  DEPENDS imux_kernel VERBATIM)
