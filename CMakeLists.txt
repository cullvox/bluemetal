cmake_minimum_required(VERSION 3.24)

project(BloodLust)

OPTION(BLOODLUST_DEBUGGING ON "Enables debugging commands in engine.")

add_subdirectory("ThirdParty")
add_subdirectory("Engine")
add_subdirectory("Game")