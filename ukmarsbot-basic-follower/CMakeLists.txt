cmake_minimum_required(VERSION 3.14)
project(ukmarsbot-basic-follower)

set(CMAKE_CXX_STANDARD 14)
add_definitions(
        -DF_CPU=16000000L
        -DARDUINO=10805
        -DARDUINO_AVR_UNO
        -DARDUINO_ARCH_AVR
        -D__AVR_ATmega328P__
)

include_directories(.)
include_directories(/Users/peterharrison/OneDrive/Documents/git-repos/Arduino_Core_avr/cores/arduino)
include_directories(/Users/peterharrison/OneDrive/Documents/git-repos/Arduino_Core_avr/variants/standard)

add_executable(ukmarsbot-basic-follower
        ukmarsbot-basic-follower.ino
        blinker.h
        board.h
        cli.cpp
        cli.h
        cli_commands.cpp
        cli_commands.h
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
        twiddle.h
        analog.cpp
        analog.h
        )
