file(GLOB SOURCES "*.cpp")

# loop through each source file
foreach(source ${SOURCES})
    # derive the basename of the source file (eg: `test.cpp` becomes `test`)
    get_filename_component(EXE_NAME ${source} NAME_WE)
    
    # see: cmake/Helpers.cmake
    add_example(${EXE_NAME} ${source})
    
    # Add assets to this example, see: cmake/Helpers.cmake
    handle_assets(${EXE_NAME} ${REPO_ROOT_DIR}/examples/assets)
endforeach()
