message("")
message("**** configuring KratosExternalSolversApplication ****")

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    message("KratosExternalSolversApplication is compiled on Android")
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.android.txt)
else()
    message("KratosExternalSolversApplication is compiled with Python 2")
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.unix.txt)
endif()

message("**** finish configuring KratosExternalSolversApplication ****")
message("")

