# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/mateus-ql/esp/esp-idf-v4.4.2/components/bootloader/subproject"
  "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader"
  "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader-prefix"
  "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader-prefix/tmp"
  "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader-prefix/src/bootloader-stamp"
  "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader-prefix/src"
  "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/mateus-ql/Documents/SmartRing_FV_FW/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
