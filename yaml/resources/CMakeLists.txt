function(ADD_RESOURCES TARGET)
    set(SOURCES)
    foreach(RESOURCE ${ARGN})
        get_filename_component(RESOURCE_NAME ${RESOURCE} NAME)
        
        file(RELATIVE_PATH RELATIVE_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}" "${RESOURCE}")
        set(OUT_F "${CMAKE_BINARY_DIR}/${RELATIVE_RESOURCE}.o")

        get_filename_component(OUT_DIR ${OUT_F} PATH)

        add_custom_command(
            OUTPUT "${OUT_F}"
            DEPENDS "${RELATIVE_RESOURCE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
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
    set(options "")
    set(singleValueArgs TYPE ARCH DESTINATION)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(LIB_DIST
        "${options}"
        "${singleValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    if(NOT TARGET_TYPE STREQUAL STATIC_LIBRARY AND
        NOT TARGET_TYPE STREQUAL SHARED_LIBRARY AND
        NOT TARGET_TYPE STREQUAL MODULE_LIBRARY)
        message(FATAL_ERROR "Target not a library: ${TARGET}")
    endif()

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}")
        message(FATAL_ERROR "The DIST_DIR was not found: ${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}")
    endif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}")

    if(NOT LIB_DIST_TYPE AND TARGET_TYPE STREQUAL STATIC_LIBRARY)
        set(LIB_DIST_TYPE STATIC)
    elseif(NOT LIB_DIST_TYPE AND TARGET_TYPE STREQUAL SHARED_LIBRARY)
        set(LIB_DIST_TYPE SHARED)
    elseif(NOT LIB_DIST_TYPE AND TARGET_TYPE STREQUAL MODULE_LIBRARY)
        set(LIB_DIST_TYPE MODULE)
    elseif(NOT ${LIB_DIST_TYPE} MATCHES STATIC AND NOT ${LIB_DIST_TYPE} MATCHES SHARED AND NOT ${LIB_DIST_TYPE} MATCHES MODULE)
        message(FATAL_ERROR "The TYPE argument must be either STATIC|SHARED|MODULE if provided. Gave: ${LIB_DIST_TYPE}")
    elseif(NOT ${LIB_DIST_TYPE}_LIBRARY MATCHES ${TARGET_TYPE})
        set(LIB_DIST_TYPE "")
    endif()

    if(NOT LIB_DIST_ARCH)
        set(LIB_DIST_ARCH amd64)
    endif(NOT LIB_DIST_ARCH)

    if(NOT LIB_DIST_DESTINATION)
        set(LIB_DIST_DESTINATION "usr/lib")
    endif(NOT LIB_DIST_DESTINATION)
    
    get_filename_component(DIST_DIR_NAME ${DIST_DIR} NAME)

    set(VERSION ${CMAKE_PROJECT_VERSION})
    if(NOT VERSION)
        set(VERSION 0.1)
    endif(NOT VERSION)
    string(REPLACE "." ";" VER_LIST ${VERSION})
    
    if((${LIB_DIST_TYPE} MATCHES SHARED) OR (${LIB_DIST_TYPE} MATCHES MODULE))
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}" "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${LIB_DIST_DESTINATION}"
            COMMAND "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:${TARGET}>"
                "${DIST_DIR}/${LIB_DIST_DESTINATION}/$<TARGET_FILE_NAME:${TARGET}>.${VERSION}"
            COMMENT "Copying shared library: ${DIST_DIR}.${VERSION}"
        )

        set(CURRENT_VER "")
        foreach(VER ${VER_LIST})
            add_custom_command(TARGET ${TARGET} POST_BUILD
                WORKING_DIRECTORY "${DIST_DIR}/${LIB_DIST_DESTINATION}"
                COMMAND "${CMAKE_COMMAND}" -E create_symlink "$<TARGET_FILE_NAME:${TARGET}>.${VERSION}" "$<TARGET_FILE_NAME:${TARGET}>${CURRENT_VER}"
                COMMENT "Symlinking: ${DIST_DIR}${CURRENT_VER}"
            )
            string(APPEND CURRENT_VER "." ${VER})
        endforeach(VER ${VER_LIST})

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${LIB_DIST_ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${LIB_DIST_ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${LIB_DIST_ARCH}.deb"
        )

        foreach(DEP ${LIB_DIST_DEPENDS})
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -DSOURCE="${DEP}" -DDEST="${DIST_DIR}/${LIB_DIST_DESTINATION}" -P "${CMAKE_SOURCE_DIR}/resources/copy_follow_symlink.cmake"
                COMMENT "Copying dependency: ${DEP}"
            )
        endforeach(DEP ${LIB_DIST_DEPENDS})
    endif()

    if(${LIB_DIST_TYPE} MATCHES STATIC)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}" "${DIST_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${LIB_DIST_DESTINATION}"
            COMMAND "${CMAKE_COMMAND}" -E copy
                "$<TARGET_FILE:${TARGET}>"
                "${DIST_DIR}/${LIB_DIST_DESTINATION}/$<TARGET_FILE_NAME:${TARGET}>"
            COMMENT "Copying static library: ${DIST_DIR}"
        )

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${LIB_DIST_ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${LIB_DIST_ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${LIB_DIST_ARCH}.deb"
        )

        foreach(DEP ${LIB_DIST_DEPENDS})
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -DSOURCE="${DEP}" -DDEST="${DIST_DIR}/${LIB_DIST_DESTINATION}" -P "${CMAKE_SOURCE_DIR}/resources/copy_follow_symlink.cmake"
                COMMENT "Copying dependency: ${DEP}"
            )
        endforeach(DEP ${LIB_DIST_DEPENDS})
    endif()
endfunction(TARGET_LIBRARY_DISTRIBUTION)

function(TARGET_FILE_DISTRIBUTION TARGET FILE DIST_DIR)
    set(options "")
    set(singleValueArgs ARCH DESTINATION)
    set(multiValueArgs DEPENDS)
    cmake_parse_arguments(FILE_DIST
        "${options}"
        "${singleValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}")
        message(FATAL_ERROR "The DIST_DIR was not found: ${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}")
    endif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DIST_DIR}")

    get_filename_component(ABS_FILE ${FILE} ABSOLUTE)

    if(NOT FILE_DIST_ARCH)
        set(FILE_DIST_ARCH amd64)
    endif(NOT FILE_DIST_ARCH)

    if(NOT FILE_DIST_DESTINATION)
        set(FILE_DIST_DESTINATION "")
    endif(NOT FILE_DIST_DESTINATION)

    get_filename_component(DIST_DIR_NAME ${DIST_DIR} NAME)

    set(VERSION ${CMAKE_PROJECT_VERSION})
    if(NOT VERSION)
        set(VERSION 0.1)
    endif(NOT VERSION)
    string(REPLACE "." ";" VER_LIST ${VERSION})

    if(IS_DIRECTORY "${ABS_FILE}")
        file(RELATIVE_PATH RELATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${ABS_FILE}")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${FILE_DIST_DESTINATION}/${RELATIVE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy_directory "${ABS_FILE}" "${DIST_DIR}/${FILE_DIST_DESTINATION}/${RELATIVE_DIR}"
            COMMENT "Copying directory: ${ABS_FILE}"
        )

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${FILE_DIST_ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${FILE_DIST_ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${FILE_DIST_ARCH}.deb"
        )

        foreach(DEP ${FILE_DIST_DEPENDS})
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -DSOURCE="${DEP}" -DDEST="${DIST_DIR}/${FILE_DIST_DESTINATION}/${RELATIVE_DIR}" -P "${CMAKE_SOURCE_DIR}/resources/copy_follow_symlink.cmake"
                COMMENT "Copying dependency: ${DEP}"
            )
        endforeach(DEP ${FILE_DIST_DEPENDS})
    else()
        get_filename_component(PARENT_DIR "${ABS_FILE}" DIRECTORY)
        file(RELATIVE_PATH RELATIVE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${PARENT_DIR}")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIST_DIR}/${FILE_DIST_DESTINATION}/${RELATIVE_DIR}"
            COMMAND "${CMAKE_COMMAND}" -E copy "${ABS_FILE}" "${DIST_DIR}/${FILE_DIST_DESTINATION}/${RELATIVE_DIR}"
            COMMENT "Copying file: ${ABS_FILE}"
        )

        add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${FILE_DIST_ARCH}.deb"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "debian"
            COMMAND "${CMAKE_COMMAND}" -E touch "debian/control"
            COMMAND dpkg-deb --build --root-owner-group "${DIST_DIR}" "${DIST_DIR_NAME}_${VERSION}_${FILE_DIST_ARCH}.deb"
            COMMENT "Building package: ${DIST_DIR_NAME}_${VERSION}_${FILE_DIST_ARCH}.deb"
        )

        foreach(DEP ${FILE_DIST_DEPENDS})
            add_custom_command(TARGET ${TARGET} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -DSOURCE="${DEP}" -DDEST="${DIST_DIR}/${FILE_DIST_DESTINATION}" -P "${CMAKE_SOURCE_DIR}/resources/copy_follow_symlink.cmake"
                COMMENT "Copying dependency: ${DEP}"
            )
        endforeach(DEP ${FILE_DIST_DEPENDS})
    endif()
endfunction(TARGET_FILE_DISTRIBUTION TARGET FILE DIST_DIR)
