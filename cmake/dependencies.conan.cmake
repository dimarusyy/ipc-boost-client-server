if(CONAN_EXPORTED) # in conan local cache
    message(STATUS "Conan : local cache")
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
else()
    message(STATUS "Conan : installing dependencies")
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD 
                "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
             EXPECTED_HASH 
                 SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
             TLS_VERIFY 
                 ON
        )
    endif()
    # conan integration
    message(STATUS "Conan : using ${CMAKE_BINARY_DIR}/conan.cmake")
    include(${CMAKE_BINARY_DIR}/conan.cmake)
    conan_check()

    conan_cmake_configure(
        REQUIRES 
            "boost/1.79.0"
        GENERATORS 
            cmake_find_package
    )

    conan_cmake_autodetect(settings)

    conan_cmake_install(
        PATH_OR_REFERENCE .
        BUILD 
            missing
        SETTINGS 
            ${settings}
    )
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})