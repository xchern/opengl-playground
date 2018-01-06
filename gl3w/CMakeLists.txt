find_package(OpenGL REQUIRED)

include_directories(
    .
    ${OPENGL_INCLUDE_DIR}
    )

add_library(gl3w
    gl3w.c
    )
target_link_libraries(gl3w
    ${OPENGL_LIBRARIES}
    )

IF(UNIX)
target_link_libraries(gl3w
    dl
    )
ENDIF(UNIX)