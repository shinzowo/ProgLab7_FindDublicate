cmake_minimum_required(VERSION 3.5.0)
project(dublicate_boost VERSION 0.1.0 LANGUAGES C CXX)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основной исполняемый файл
add_executable(dublicate_boost main.cpp)

# Опции компилятора для поддержки концептов в GCC
target_compile_options(dublicate_boost PRIVATE -fconcepts)

# Устанавливаем путь для исполняемых файлов при установке пакета
install(TARGETS dublicate_boost DESTINATION bin)

# Включаем CPack для создания пакетов (DEB и ZIP)
set(CPACK_GENERATOR "DEB;ZIP")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)


