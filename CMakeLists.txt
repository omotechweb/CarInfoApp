cmake_minimum_required(VERSION 3.16)

project(MyQtApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_WIN32_EXECUTABLE TRUE)  # Windows GUI için

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(MyQtApp
    main.cpp
)

target_link_libraries(MyQtApp PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
