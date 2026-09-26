file(GLOB_RECURSE AION_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp")
file(GLOB_RECURSE AION_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/public/*.h")

add_library(Aion STATIC ${AION_SOURCES} ${AION_HEADERS})

target_include_directories(Aion PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")

# Public headers expose sf:: types, so SFML is part of the interface.
target_link_libraries(Aion
    PUBLIC  SFML::Graphics SFML::Window SFML::System
    PRIVATE AionWarnings)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${AION_SOURCES} ${AION_HEADERS})
