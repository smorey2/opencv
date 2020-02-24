cmake_minimum_required(VERSION 3.8.0)
project(cv)

# WINDOWS
if(WIN32)
   set(VCPKG ${CMAKE_SOURCE_DIR}/_vcpkg)

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
endif()

# cv
find_package(OpenCV REQUIRED)
add_executable(cv PUBLIC program.cpp)
target_link_libraries(cv ${OpenCV_LIBS})