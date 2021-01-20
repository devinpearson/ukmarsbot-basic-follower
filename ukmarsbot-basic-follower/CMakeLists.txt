cmake_minimum_required(VERSION 3.14)
project(ukmarsbot_system_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)
add_definitions(
        -DF_CPU=16000000L
        -DARDUINO=10805
        -DARDUINO_AVR_UNO
        -DARDUINO_ARCH_AVR
)

include_directories(.)
include_directories(/Users/peterharrison/Dropbox/git-repos/Arduino_Core_avr/cores/arduino)
include_directories(/Users/peterharrison/Dropbox/git-repos/Arduino_Core_avr/variants/standard)

add_executable(ukmarsbot_system_test
        ukmarsbot-system-test.ino
        blinker.h
        board.h
        cli.cpp
        cli.h
        commands.cpp
        commands.h
        config.h
        defaults.h
        encoders.cpp
        encoders.h
        motion.cpp
        motion.h
        motors.cpp
        motors.h
        sensors.cpp
        sensors.h
        settings.cpp
        settings.h
        streaming.h
        systick.cpp
        systick.h
        twiddle.cpp
        twiddle.h)
