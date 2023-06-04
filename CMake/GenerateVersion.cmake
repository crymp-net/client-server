execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty --tags --match "v*"
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	RESULT_VARIABLE GIT_DESCRIBE_STATUS
	OUTPUT_VARIABLE GIT_DESCRIBE_OUTPUT
	ERROR_VARIABLE GIT_DESCRIBE_ERROR
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GIT_DESCRIBE_STATUS AND NOT GIT_DESCRIBE_STATUS EQUAL 0)
	message(FATAL_ERROR "Failed to obtain project version from Git!\n${GIT_DESCRIBE_ERROR}")
endif()

set(CRYMP_CLIENT_VERSION_STRING "${GIT_DESCRIBE_OUTPUT}")

# parse version number
string(REGEX REPLACE "^v([0-9]+).*$" "\\1" CRYMP_CLIENT_VERSION "${CRYMP_CLIENT_VERSION_STRING}")

configure_file(version.h.in ${PROJECT_BINARY_DIR}/new_version.h)

# trigger rebuild only if needed
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different new_version.h version.h
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
file(REMOVE ${PROJECT_BINARY_DIR}/new_version.h)
