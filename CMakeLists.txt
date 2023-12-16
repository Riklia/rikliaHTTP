# CMakeList.txt : CMake project for rikliaHTTP, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("rikliaHTTP")

set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)

find_package(ZLIB REQUIRED)

include_directories(inc)
include_directories(src)

add_executable (rikliaHTTP "main.cpp"  "src/TCPServer.cpp" "inc/TCPServer.h" "src/HTTPMessage.cpp" "inc/HTTPMessage.h" "src/utils/string_utils.cpp" "src/utils/string_utils.h" "src/utils/file_utils.cpp" "src/utils/file_utils.h" "inc/ServerConfiguration.h" "inc/GlobalConfigurations.h" "src/GlobalConfigurations.cpp" "inc/HTTPRequest.h" "src/HTTPRequest.cpp" "src/HTTPResponse.cpp" "inc/HTTPResponse.h" "inc/HTTPServer.h" "src/HTTPServer.cpp" "src/ServerConfiguration.cpp" "inc/ServerLogger.h" "src/ServerLogger.cpp")

target_link_libraries(rikliaHTTP PRIVATE Threads::Threads)
target_link_libraries(rikliaHTTP PRIVATE ${ZLIB_LIBRARIES})