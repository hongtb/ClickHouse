# This strings autochanged from release_lib.sh:
set(VERSION_REVISION 54416)
set(VERSION_MAJOR 19)
set(VERSION_MINOR 4)
set(VERSION_PATCH 1)
set(VERSION_GITHASH 628ed349c335b79a441a1bd6e4bc791d61dfe62c)
set(VERSION_DESCRIBE v19.4.1.1-testing)
set(VERSION_STRING 19.4.1.1)
# end of autochange

set(VERSION_EXTRA "" CACHE STRING "")
set(VERSION_TWEAK "" CACHE STRING "")

if (VERSION_TWEAK)
    string(CONCAT VERSION_STRING ${VERSION_STRING} "." ${VERSION_TWEAK})
endif ()

if (VERSION_EXTRA)
    string(CONCAT VERSION_STRING ${VERSION_STRING} "." ${VERSION_EXTRA})
endif ()

set (VERSION_NAME "${PROJECT_NAME}")
set (VERSION_FULL "${VERSION_NAME} ${VERSION_STRING}")
set (VERSION_SO "${VERSION_STRING}")

math (EXPR VERSION_INTEGER "${VERSION_PATCH} + ${VERSION_MINOR}*1000 + ${VERSION_MAJOR}*1000000")
