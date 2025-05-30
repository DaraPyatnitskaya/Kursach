cmake_minimum_required(VERSION 3.10)
project(kursach)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 17)

# Добавляем директории для поиска заголовочных файлов
include_directories(include)

# Устанавливаем путь к директории с активами
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(DATABASE_DIR "${CMAKE_SOURCE_DIR}/database")

# Добавляем все исходные файлы и данные
file(GLOB SOURCES
        src/*.cpp
        main.cpp
        src/Game.cpp
        include/Game.hpp
)

# Указываем путь к SFML
set(SFML_DIR "/opt/homebrew//Cellar/sfml@2/2.6.2_1/lib/cmake/SFML")

# Находим нужные компоненты SFML, включая audio
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system audio)

# Создаём исполняемый файл
add_executable(ClickerGame ${SOURCES})

# Линкуем библиотеки SFML и SQLite
target_link_libraries(ClickerGame sfml-graphics sfml-window sfml-system sfml-audio SQLite3)

# Указываем необходимые ресурсы для копирования
configure_file(${ASSETS_DIR}/font.ttf ${CMAKE_BINARY_DIR}/font.ttf COPYONLY)
configure_file(${ASSETS_DIR}/cat.png ${CMAKE_BINARY_DIR}/cat.png COPYONLY)
configure_file(${ASSETS_DIR}/coins.png ${CMAKE_BINARY_DIR}/coins.png COPYONLY)
configure_file(${ASSETS_DIR}/feed_button.png ${CMAKE_BINARY_DIR}/feed_button.png COPYONLY)
configure_file(${ASSETS_DIR}/andrey.png ${CMAKE_BINARY_DIR}/andrey.png COPYONLY)
configure_file(${ASSETS_DIR}/andrey_talk.png ${CMAKE_BINARY_DIR}/andrey_talk.png COPYONLY)
configure_file(${ASSETS_DIR}/background.png ${CMAKE_BINARY_DIR}/background.png COPYONLY)
configure_file(${ASSETS_DIR}/background_music.wav ${CMAKE_BINARY_DIR}/background_music.wav COPYONLY)
configure_file(${ASSETS_DIR}/coin.wav ${CMAKE_BINARY_DIR}/coin.wav COPYONLY)
configure_file(${ASSETS_DIR}/purr.wav ${CMAKE_BINARY_DIR}/purr.wav COPYONLY)
configure_file(${DATABASE_DIR}/phrases.db ${CMAKE_BINARY_DIR}/phrases.db COPYONLY)

# Копируем картины (painting1.png ... painting10.png)
foreach(i RANGE 1 10)
    configure_file(${ASSETS_DIR}/painting${i}.png ${CMAKE_BINARY_DIR}/painting${i}.png COPYONLY)
endforeach()

# Копируем всю папку assets после сборки
add_custom_command(TARGET ClickerGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:ClickerGame>/assets)
