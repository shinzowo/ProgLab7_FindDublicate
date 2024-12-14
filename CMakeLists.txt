cmake_minimum_required(VERSION 3.5.0)
project(dublicate_boost VERSION 0.1.0 LANGUAGES C CXX)
#set(BOOST_ROOT "C:/boost-build")
#set(BOOST_INCLUDE_DIR "C:/boost-build/include")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
include_directories(${BOOST_ROOT})
find_package(Boost REQUIRED COMPONENTS filesystem program_options)
add_executable(dublicate_boost main.cpp)


if(Boost_FOUND)
    target_include_directories(dublicate_boost PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(dublicate_boost PUBLIC Boost::filesystem Boost::program_options)
else()
    message(FATAL_ERROR "Boost not found!")
endif()
install(TARGETS dublicate_boost RUNTIME DESTINATION bin)
# Устанавливаем настройки для упаковки (если нужно)
set(CPACK_PACKAGE_NAME "dublicate_boost")
set(CPACK_GENERATOR "DEB;ZIP")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)


