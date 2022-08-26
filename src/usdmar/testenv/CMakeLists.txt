add_subdirectory(stageResolve)




# Is there a better way to do this?
find_library(_CPR_LIB_PATH cpr)
get_filename_component(_CPR_ROOT ${_CPR_LIB_PATH} DIRECTORY)
get_filename_component(_CPR_ROOT ${_CPR_ROOT} DIRECTORY)

set(_ENV_PYTHONPATH "PYTHONPATH=${pxr_DIR}/lib/python\;${PYTHONPATH}")
set(_ENV_PATH "PATH=${PATH}\;${pxr_DIR}/lib\;${pxr_DIR}/bin\;${_CPR_ROOT}/bin\;${_CPR_ROOT}/lib")
set(_ENV_PXR_PLUGINPATH_NAME "PXR_PLUGINPATH_NAME=${CMAKE_BINARY_DIR}")
set(_ENV_USDMULTISOLVER_CONFIG_PATH "USDMULTISOLVER_CONFIG_PATH=${CMAKE_CURRENT_LIST_DIR}/resources/config.json")

add_test(NAME tests
	COMMAND  ${CMAKE_COMMAND} -E env ${_ENV_PYTHONPATH} ${_ENV_PATH} ${_ENV_PXR_PLUGINPATH_NAME} ${_ENV_USDMULTISOLVER_CONFIG_PATH}  ${Python3_EXECUTABLE} -m unittest discover -s ${CMAKE_CURRENT_SOURCE_DIR}
)