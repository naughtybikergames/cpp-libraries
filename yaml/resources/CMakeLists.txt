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

function(TARGET_LIBRARY_DISTRIBUTION TARGET DIST_DIR)
    set(LIB_FLAG ${ARGV2})
    if(NOT LIB_FLAG)
        set(LIB_FLAG STATIC)
    endif(NOT LIB_FLAG)

    set(ARCH ${ARGV3})
    if(NOT ARCH)
        set(ARCH amd64)
    endif(NOT ARCH)

    set(LIB_DEST ${ARGV4})
    if(NOT LIB_DEST)
        set(LIB_DEST "usr/lib")
    endif(NOT LIB_DEST)
    
    get_filename_component(DIST_DIR_NAME ${DIST_DIR} NAME)

    set(VERSION ${CMAKE_PROJECT_VERSION})
    if(NOT VERSION)
        set(VERSION 0.1)
    endif(NOT VERSION)
    string(REPLACE "." ";" VER_LIST ${VERSION})

    if(BUILD_SHARED_LIBS AND ${LIB_FLAG} MATCHES SHARED)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/${DIST_DIR}" "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${LIB_DEST}"
            COMMAND "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:${TARGET}>"
                "${DIST_DIR}/${LIB_DEST}/$<TARGET_FILE_NAME:${TARGET}>.${VERSION}"
            COMMENT "Copying shared library: ${DIST_DIR}.${VERSION}"
        )

        set(CURRENT_VER "")
        foreach(VER ${VER_LIST})
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E create_symlink "${DIST_DIR}/${LIB_DEST}/$<TARGET_FILE_NAME:${TARGET}>.${VERSION}" "${DIST_DIR}/${LIB_DEST}/$<TARGET_FILE_NAME:${TARGET}>${CURRENT_VER}"
                COMMENT "Symlinking: ${DIST_DIR}${CURRENT_VER}"
            )
            string(APPEND CURRENT_VER "." ${VER})
        endforeach(VER ${VER_LIST})

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
        )
    endif()

    if(NOT BUILD_SHARED_LIBS AND ${LIB_FLAG} MATCHES STATIC)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_SOURCE_DIR}/${DIST_DIR}" "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${LIB_DEST}"
            COMMAND "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:${TARGET}>"
                "${DIST_DIR}/${LIB_DEST}/$<TARGET_FILE_NAME:${TARGET}>"
            COMMENT "Copying static library: ${DIST_DIR}"
        )

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
        )
    endif()
endfunction(TARGET_LIBRARY_DISTRIBUTION)

function(TARGET_FILE_DISTRIBUTION TARGET FILE DIST_DIR DEST_PREFIX)
    get_filename_component(ABS_FILE ${FILE} ABSOLUTE)

    set(ARCH ${ARGV4})
    if(NOT ARCH)
        set(ARCH amd64)
    endif(NOT ARCH)

    get_filename_component(DIST_DIR_NAME ${DIST_DIR} NAME)

    set(VERSION ${CMAKE_PROJECT_VERSION})
    if(NOT VERSION)
        set(VERSION 0.1)
    endif(NOT VERSION)
    string(REPLACE "." ";" VER_LIST ${VERSION})

    if(IS_DIRECTORY "${ABS_FILE}")
        file(RELATIVE_PATH RELATIVE_DIR "${CMAKE_SOURCE_DIR}" "${ABS_FILE}")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${DEST_PREFIX}/${RELATIVE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${ABS_FILE}" "${DIST_DIR}/${DEST_PREFIX}/${RELATIVE_DIR}"
            COMMENT "Copying directory: ${ABS_FILE}"
        )

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
        )
    else()
        get_filename_component(PARENT_DIR "${ABS_FILE}" DIRECTORY)
        file(RELATIVE_PATH RELATIVE_DIR "${CMAKE_SOURCE_DIR}" "${PARENT_DIR}")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${DEST_PREFIX}/${RELATIVE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${ABS_FILE}" "${DIST_DIR}/${DEST_PREFIX}/${RELATIVE_DIR}"
            COMMENT "Copying file: ${ABS_FILE}"
        )

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${ARCH}.deb"
        )
    endif()
endfunction(TARGET_FILE_DISTRIBUTION TARGET FILE DIST_DIR DEST_PREFIX)
