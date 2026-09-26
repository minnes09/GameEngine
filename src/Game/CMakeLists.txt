file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp")
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h")

# No entry point yet: built as a library until the game gets its main().
add_library(Game STATIC ${GAME_SOURCES} ${GAME_HEADERS})

target_include_directories(Game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")

target_link_libraries(Game
    PUBLIC  Aion
    PRIVATE AionWarnings)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GAME_SOURCES} ${GAME_HEADERS})

add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/app/Main.cpp")

target_link_libraries(Sandbox
    PRIVATE Game
            AionWarnings)

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT Sandbox)
set_target_properties(Sandbox PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
