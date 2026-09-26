file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp")
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h")

add_executable(Game ${GAME_SOURCES} ${GAME_HEADERS})

target_include_directories(Game PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")

target_link_libraries(Game
    PRIVATE Myrias
            MyriasWarnings)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${GAME_SOURCES} ${GAME_HEADERS})

set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT Game)
set_target_properties(Game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
