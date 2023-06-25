set(IPC_BOOST_CLIENT ipc-boost-client)

add_executable(${IPC_BOOST_CLIENT}
    main.cpp
)

target_link_libraries(${IPC_BOOST_CLIENT} 
    PRIVATE
        Boost::boost    
)

install(
    TARGETS 
        ${IPC_BOOST_CLIENT} 
    LIBRARY
        DESTINATION bin
)
