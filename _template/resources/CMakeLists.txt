function(ADD_RESOURCES TARGET)
    set(SOURCES)
    foreach(RESOURCE ${ARGN})
        get_filename_component(RESOURCE_NAME ${RESOURCE} NAME)
        
        file(RELATIVE_PATH RELATIVE_RESOURCE "${CMAKE_SOURCE_DIR}" "${RESOURCE}")
        set(OUT_F "${CMAKE_BINARY_DIR}/${RELATIVE_RESOURCE}.o")

        get_filename_component(OUT_DIR ${OUT_F} PATH)

        add_custom_command(
            OUTPUT "${OUT_F}"
            DEPENDS "${RELATIVE_RESOURCE}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMAND ${CMAKE_COMMAND}
            ARGS -E make_directory ${OUT_DIR}
            COMMAND ${CMAKE_OBJCOPY}
            ARGS --input binary --output elf64-x86-64 "${RELATIVE_RESOURCE}" "${OUT_F}"
            COMMENT "Objcopying ${RELATIVE_RESOURCE}"
        )

        list(APPEND SOURCES "${OUT_F}")
    endforeach(RESOURCE)

    target_sources(${TARGET}
        PRIVATE
            ${SOURCES}
    )
endfunction(ADD_RESOURCES)