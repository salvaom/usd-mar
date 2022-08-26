
set(_RESOLVER_SCHEMAS "")
foreach(X IN LISTS RESOLVER_SCHEMAS)
    string(APPEND _RESOLVER_SCHEMAS "\"${X}\",")
endforeach()
string(REGEX REPLACE ",$" "" _RESOLVER_SCHEMAS "${_RESOLVER_SCHEMAS}")


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
	"solvers/subsolver.h"
	"solvers/rest.cpp"
	"solvers/rest.h"
	"solvers/env.h"
	"solvers/env.cpp"
	"solvers/format.h"
	"solvers/format.cpp"
)
target_include_directories(${PROJECT_NAME} 
	PRIVATE
		${PXR_INCLUDE_DIRS}
		${CPR_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
	${PXR_LIBRARIES}
	cpr::cpr
)

configure_file(
    "plugInfo.json"
    ${CMAKE_BINARY_DIR}/plugInfo.json
)

add_subdirectory(testenv)

install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Config"
    RUNTIME DESTINATION usd ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION include
)
install(FILES ${CMAKE_BINARY_DIR}/plugInfo.json DESTINATION ./usd)
install(FILES ${USDMAR_HEADERS} DESTINATION "include/usdmar")

