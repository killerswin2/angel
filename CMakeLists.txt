cmake_minimum_required(VERSION 3.24.2)

project(SQAS)
file(GLOB SOURCES "src/*.hpp" "src/*.cpp")


set(ADD_ON_PATH "lib/add_on")
file(GLOB_RECURSE ADDON_SOURCES 
        "${ADD_ON_PATH}/contextmgr/*.h" "${ADD_ON_PATH}/contextmgr/*.cpp"
        "${ADD_ON_PATH}/datatime/*.h" "${ADD_ON_PATH}/datetime/*.cpp"
        "${ADD_ON_PATH}/debugger/*.h" "${ADD_ON_PATH}/debugger/*.cpp"
        "${ADD_ON_PATH}/scriptany/*.h" "${ADD_ON_PATH}/scriptany/*.cpp"
        "${ADD_ON_PATH}/scriptarray/*.h" "${ADD_ON_PATH}/scriptarray/*.cpp"
        "${ADD_ON_PATH}/scriptbuilder/*.h" "${ADD_ON_PATH}/scriptbuilder/*.cpp"
        "${ADD_ON_PATH}/scriptdictionary/*.h" "${ADD_ON_PATH}/scriptdictionary/*.cpp"
        "${ADD_ON_PATH}/scriptfile/*.h" "${ADD_ON_PATH}/scriptfile/*.cpp"
        "${ADD_ON_PATH}/scriptgrid/*.h" "${ADD_ON_PATH}/scriptgrid/*.cpp" src/json_object.cpp
        "${ADD_ON_PATH}/scripthandle/*.h" "${ADD_ON_PATH}/scripthandle/*.cpp"
        "${ADD_ON_PATH}/scripthelper/*.h" "${ADD_ON_PATH}/scripthelper/*.cpp"
        "${ADD_ON_PATH}/scriptmath/*.h" "${ADD_ON_PATH}/scriptmath/*.cpp"
        "${ADD_ON_PATH}/scriptstdstring/*.h" "${ADD_ON_PATH}/scriptstdstring/*.cpp"
        "${ADD_ON_PATH}/serializer/*.h" "${ADD_ON_PATH}/serializer/*.cpp"
        "${ADD_ON_PATH}/weakref/*.h" "${ADD_ON_PATH}/weakref/*.cpp")


find_package(Angelscript REQUIRED)

add_executable(SQAS ${SOURCES} ${ADDON_SOURCES})
set_property(TARGET SQAS PROPERTY CXX_STANDARD 17)
target_include_directories(SQAS PUBLIC ${ADD_ON_PATH} PUBLIC "/usr/local/include" "${CMAKE_SOURCE_DIR}/lib")
target_link_directories(SQAS PUBLIC "/usr/local/lib")
target_link_libraries(SQAS PUBLIC angelscript)
