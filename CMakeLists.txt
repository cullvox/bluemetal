cmake_minimum_required(VERSION 3.10)

project(BloodLust)

OPTION(BLOODLUST_USE_PACKAGE_MANAGER ON "Uses the system to find packages.")
OPTION(BLOODLUST_DEBUGGING ON "Enables debugging commands in engine.")

find_package(PkgConfig REQUIRED)

add_subdirectory("ThirdParty")
add_subdirectory("Engine")
add_subdirectory("Game")