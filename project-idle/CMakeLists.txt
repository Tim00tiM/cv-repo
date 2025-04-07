cmake_minimum_required(VERSION 3.20)
project(idle-game)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

FILE(COPY res DESTINATION "${CMAKE_BINARY_DIR}")

add_executable( idle-game
        main.cpp
        src/Generator.h
        src/Generator.cpp
        src/Player.h
        src/Player.cpp
        src/Observable.h
        src/Observer.h
        views/Component.h
        views/Composite.cpp
        views/Composite.h
        views/TextComponent.cpp
        views/TextComponent.h
        views/Component.cpp
        views/ObserverTextComponent.cpp
        views/ObserverTextComponent.h
        res/Colors.cpp
        res/Fonts.cpp
        res/Fonts.h
        views/RectComponent.cpp
        views/RectComponent.h
        views/ImageComponent.cpp
        views/ImageComponent.h
        views/ButtonComponent.h
        views/ButtonComponent.cpp
        util/BigInteger.cpp
        util/BigInteger.h
        views/RectWithTextComponent.cpp
        views/RectWithTextComponent.h
        res/Strings.cpp
)

target_link_libraries(idle-game sfml-graphics sfml-audio)
