
set(_RESOLVER_SCHEMES "")
foreach(X IN LISTS RESOLVER_SCHEMES)
    string(APPEND _RESOLVER_SCHEMES "\"${X}\",")
endforeach()
string(REGEX REPLACE ",$" "" _RESOLVER_SCHEMES "${_RESOLVER_SCHEMES}")

configure_file(
	"config.h.in"
	"config.h"
)

add_library(${PROJECT_NAME}
SHARED
	"resolver.cpp"
	"resolver.h"
	"utils.cpp"
	"solverStack.h"
	"solverStack.cpp"
	"registry.h"
	"registry.cpp"
	"utils.h"
	"debug.h"
	"config.h"
	"solvers/subsolver.h"
	"solvers/env.h"
	"solvers/env.cpp"
	"solvers/format.h"
	"solvers/format.cpp"
)



if (USDMAR_REST)
	target_sources(${PROJECT_NAME} 
	PRIVATE
		"solvers/rest.h"
		"solvers/rest.cpp"
	)
endif()

target_include_directories(${PROJECT_NAME} 
	PRIVATE
		${PXR_INCLUDE_DIRS}
		${CPR_INCLUDE_DIRS}
		"${CMAKE_BINARY_DIR}/src"
)


target_link_libraries(${PROJECT_NAME}
	${PXR_LIBRARIES}
)
if (USDMAR_REST)
	target_link_libraries(${PROJECT_NAME}
		cpr::cpr
	)
endif()

configure_file(
    "plugInfo.json"
    ${CMAKE_BINARY_DIR}/plugInfo.json
)

# TODO: Enable tests
# add_subdirectory(testenv)

install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Config"
    RUNTIME DESTINATION usd ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION include
)
install(FILES ${CMAKE_BINARY_DIR}/plugInfo.json DESTINATION ./usd)
install(FILES ${USDMAR_HEADERS} DESTINATION "include/usdmar")

