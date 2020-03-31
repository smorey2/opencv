cmake_minimum_required(VERSION 3.8.0)
project(cv)

set(VCPKG ${CMAKE_SOURCE_DIR}/_vcpkg)
if (WIN32)
   # install vcpkg
   if (NOT IS_DIRECTORY ${VCPKG})
      message("vcpkg does not exist. installing...")
      execute_process(
         COMMAND git.exe clone https://github.com/Microsoft/vcpkg.git ${VCPKG}
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      )
      execute_process(
         COMMAND bootstrap-vcpkg.bat
         WORKING_DIRECTORY ${VCPKG}
      )
   endif()

   # install opencv:x64-windows
   if (NOT IS_DIRECTORY ${VCPKG}/packages/opencv_x64-windows)
      execute_process(
         COMMAND vcpkg install opencv:x64-windows
         WORKING_DIRECTORY ${VCPKG}
      )
   endif()

   # install xtensor:x64-windows
   if (NOT IS_DIRECTORY ${VCPKG}/packages/xtensor_x64-windows)
      execute_process(
         COMMAND vcpkg install xtensor:x64-windows
         WORKING_DIRECTORY ${VCPKG}
      )
   endif()
endif()
set(CMAKE_TOOLCHAIN_FILE "${VCPKG}/scripts/buildsystems/vcpkg.cmake") 

# cv
add_executable(cv src/program.cpp)