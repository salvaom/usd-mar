find_path(SUBPROCESSH_INCLUDE
    NAMES "subprocess.h"
    DOC "subprocess.h include dir")

find_package_handle_standard_args(SUBPROCESSH REQUIRED_VARS SUBPROCESSH_HEADER_PATH)
