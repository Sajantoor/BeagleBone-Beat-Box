# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.cpp")
add_executable(beatbox ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(beatbox LINK_PRIVATE hal)

# Copy executable to final location (change `beatbox` to project name as needed)
add_custom_command(TARGET beatbox POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:beatbox>"
     "~/cmpt433/public/myApps/" 
  COMMENT "Copying ARM executable to public NFS directory")

  # Copy the folder of WAVE files to NFS
add_custom_command(TARGET beatbox POST_BUILD
   COMMAND "${CMAKE_COMMAND}" -E copy_directory
      "${CMAKE_SOURCE_DIR}/wave-files"
      "~/cmpt433/public/myApps/beatbox-wav-files"
   COMMENT "Copying WAVE files to public NFS directory")