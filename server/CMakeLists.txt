set(IPC_BOOST_SERVER ipc-boost-server)

add_executable(${IPC_BOOST_SERVER}
    main.cpp
)

target_link_libraries(${IPC_BOOST_SERVER} 
    PRIVATE
        Boost::boost    
)

install(
    TARGETS 
        ${IPC_BOOST_SERVER} 
    LIBRARY
        DESTINATION bin
)
