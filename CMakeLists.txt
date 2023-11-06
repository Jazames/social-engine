cmake_minimum_required(VERSION 3.13)

project(Social C CXX)


if (EMSCRIPTEN)
    set(BUILD_SHARED_LIBS_DEFAULT OFF)

    option(LLAMA_WASM_SINGLE_FILE "llama: embed WASM inside the generated llama.js" ON)
else()
    if (MINGW)
        set(BUILD_SHARED_LIBS_DEFAULT OFF)
    else()
        set(BUILD_SHARED_LIBS_DEFAULT ON)
    endif()
endif()

#
# Compile flags
#

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include(CheckCXXCompilerFlag)

if (NOT MSVC)
    if (LLAMA_SANITIZE_THREAD)
        add_compile_options(-fsanitize=thread)
        link_libraries(-fsanitize=thread)
    endif()

    if (LLAMA_SANITIZE_ADDRESS)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        link_libraries(-fsanitize=address)
    endif()

    if (LLAMA_SANITIZE_UNDEFINED)
        add_compile_options(-fsanitize=undefined)
        link_libraries(-fsanitize=undefined)
    endif()
endif()

if (APPLE AND LLAMA_ACCELERATE)
    find_library(ACCELERATE_FRAMEWORK Accelerate)
    if (ACCELERATE_FRAMEWORK)
        message(STATUS "Accelerate framework found")

        add_compile_definitions(GGML_USE_ACCELERATE)
        add_compile_definitions(ACCELERATE_NEW_LAPACK)
        add_compile_definitions(ACCELERATE_LAPACK_ILP64)
        set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS} ${ACCELERATE_FRAMEWORK})
    else()
        message(WARNING "Accelerate framework not found")
    endif()
endif()

if (LLAMA_METAL)
    find_library(FOUNDATION_LIBRARY         Foundation              REQUIRED)
    find_library(METAL_FRAMEWORK            Metal                   REQUIRED)
    find_library(METALKIT_FRAMEWORK         MetalKit                REQUIRED)

    message(STATUS "Metal framework found")
    set(GGML_HEADERS_METAL ggml-metal.h)
    set(GGML_SOURCES_METAL ggml-metal.m)

    add_compile_definitions(GGML_USE_METAL)
    if (LLAMA_METAL_NDEBUG)
        add_compile_definitions(GGML_METAL_NDEBUG)
    endif()

    # get full path to the file
    #add_compile_definitions(GGML_METAL_DIR_KERNELS="${CMAKE_CURRENT_SOURCE_DIR}/")

    # copy ggml-metal.metal to bin directory
    configure_file(ggml-metal.metal bin/ggml-metal.metal COPYONLY)

    set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS}
        ${FOUNDATION_LIBRARY}
        ${METAL_FRAMEWORK}
        ${METALKIT_FRAMEWORK}
        )
endif()
if (LLAMA_BLAS)
    if (LLAMA_STATIC)
        set(BLA_STATIC ON)
    endif()
    if ($(CMAKE_VERSION) VERSION_GREATER_EQUAL 3.22)
        set(BLA_SIZEOF_INTEGER 8)
    endif()

    set(BLA_VENDOR ${LLAMA_BLAS_VENDOR})
    find_package(BLAS)

    if (BLAS_FOUND)
        message(STATUS "BLAS found, Libraries: ${BLAS_LIBRARIES}")

        if ("${BLAS_INCLUDE_DIRS}" STREQUAL "")
            # BLAS_INCLUDE_DIRS is missing in FindBLAS.cmake.
            # see https://gitlab.kitware.com/cmake/cmake/-/issues/20268
            find_package(PkgConfig REQUIRED)
            if (${LLAMA_BLAS_VENDOR} MATCHES "Generic")
                pkg_check_modules(DepBLAS REQUIRED blas)
            elseif (${LLAMA_BLAS_VENDOR} MATCHES "OpenBLAS")
                pkg_check_modules(DepBLAS REQUIRED openblas)
            elseif (${LLAMA_BLAS_VENDOR} MATCHES "FLAME")
                pkg_check_modules(DepBLAS REQUIRED blis)
            elseif (${LLAMA_BLAS_VENDOR} MATCHES "ATLAS")
                pkg_check_modules(DepBLAS REQUIRED blas-atlas)
            elseif (${LLAMA_BLAS_VENDOR} MATCHES "FlexiBLAS")
                pkg_check_modules(DepBLAS REQUIRED flexiblas_api)
            elseif (${LLAMA_BLAS_VENDOR} MATCHES "Intel")
                # all Intel* libraries share the same include path
                pkg_check_modules(DepBLAS REQUIRED mkl-sdl)
            elseif (${LLAMA_BLAS_VENDOR} MATCHES "NVHPC")
                # this doesn't provide pkg-config
                # suggest to assign BLAS_INCLUDE_DIRS on your own
                if ("${NVHPC_VERSION}" STREQUAL "")
                    message(WARNING "Better to set NVHPC_VERSION")
                else()
                    set(DepBLAS_FOUND ON)
                    set(DepBLAS_INCLUDE_DIRS "/opt/nvidia/hpc_sdk/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR}/${NVHPC_VERSION}/math_libs/include")
                endif()
            endif()
            if (DepBLAS_FOUND)
                set(BLAS_INCLUDE_DIRS ${DepBLAS_INCLUDE_DIRS})
            else()
                message(WARNING "BLAS_INCLUDE_DIRS neither been provided nor been automatically"
                " detected by pkgconfig, trying to find cblas.h from possible paths...")
                find_path(BLAS_INCLUDE_DIRS
                    NAMES cblas.h
                    HINTS
                        /usr/include
                        /usr/local/include
                        /usr/include/openblas
                        /opt/homebrew/opt/openblas/include
                        /usr/local/opt/openblas/include
                        /usr/include/x86_64-linux-gnu/openblas/include
                )
            endif()
        endif()

        message(STATUS "BLAS found, Includes: ${BLAS_INCLUDE_DIRS}")
        add_compile_options(${BLAS_LINKER_FLAGS})
        add_compile_definitions(GGML_USE_OPENBLAS)
        if (${BLAS_INCLUDE_DIRS} MATCHES "mkl" AND (${LLAMA_BLAS_VENDOR} MATCHES "Generic" OR ${LLAMA_BLAS_VENDOR} MATCHES "Intel"))
            add_compile_definitions(GGML_BLAS_USE_MKL)
        endif()
        set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS} ${BLAS_LIBRARIES})
        set(LLAMA_EXTRA_INCLUDES ${LLAMA_EXTRA_INCLUDES} ${BLAS_INCLUDE_DIRS})

    else()
        message(WARNING "BLAS not found, please refer to "
        "https://cmake.org/cmake/help/latest/module/FindBLAS.html#blas-lapack-vendors"
        " to set correct LLAMA_BLAS_VENDOR")
    endif()
endif()

if (LLAMA_K_QUANTS)
    set(GGML_HEADERS_EXTRA k_quants.h)
    set(GGML_SOURCES_EXTRA k_quants.c)
    add_compile_definitions(GGML_USE_K_QUANTS)
    if (LLAMA_QKK_64)
        add_compile_definitions(GGML_QKK_64)
    endif()
endif()

if (LLAMA_CUBLAS)
    cmake_minimum_required(VERSION 3.17)

    find_package(CUDAToolkit)
    if (CUDAToolkit_FOUND)
        message(STATUS "cuBLAS found")

        enable_language(CUDA)

        set(GGML_HEADERS_CUDA ggml-cuda.h)
        set(GGML_SOURCES_CUDA ggml-cuda.cu)

        add_compile_definitions(GGML_USE_CUBLAS)
#        if (LLAMA_CUDA_CUBLAS)
#            add_compile_definitions(GGML_CUDA_CUBLAS)
#        endif()
        if (LLAMA_CUDA_FORCE_DMMV)
            add_compile_definitions(GGML_CUDA_FORCE_DMMV)
        endif()
        add_compile_definitions(GGML_CUDA_DMMV_X=${LLAMA_CUDA_DMMV_X})
        add_compile_definitions(GGML_CUDA_MMV_Y=${LLAMA_CUDA_MMV_Y})
        if (DEFINED LLAMA_CUDA_DMMV_Y)
            add_compile_definitions(GGML_CUDA_MMV_Y=${LLAMA_CUDA_DMMV_Y}) # for backwards compatibility
        endif()
        if (LLAMA_CUDA_F16 OR LLAMA_CUDA_DMMV_F16)
            add_compile_definitions(GGML_CUDA_F16)
        endif()
        add_compile_definitions(K_QUANTS_PER_ITERATION=${LLAMA_CUDA_KQUANTS_ITER})
        add_compile_definitions(GGML_CUDA_PEER_MAX_BATCH_SIZE=${LLAMA_CUDA_PEER_MAX_BATCH_SIZE})

        if (LLAMA_STATIC)
            set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS} CUDA::cudart_static CUDA::cublas_static CUDA::cublasLt_static)
        else()
            set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS} CUDA::cudart CUDA::cublas CUDA::cublasLt)
        endif()

    if (NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
        # 52 == lowest CUDA 12 standard
        # 60 == f16 CUDA intrinsics
        # 61 == integer CUDA intrinsics
        # 70 == compute capability at which unrolling a loop in mul_mat_q kernels is faster
        if (LLAMA_CUDA_F16 OR LLAMA_CUDA_DMMV_F16)
            set(CMAKE_CUDA_ARCHITECTURES "60;61;70") # needed for f16 CUDA intrinsics
        else()
            set(CMAKE_CUDA_ARCHITECTURES "52;61;70") # lowest CUDA 12 standard + lowest for integer intrinsics
        endif()
    endif()
    message(STATUS "Using CUDA architectures: ${CMAKE_CUDA_ARCHITECTURES}")

    else()
        message(WARNING "cuBLAS not found")
    endif()
endif()

if (LLAMA_MPI)
    cmake_minimum_required(VERSION 3.10)
    find_package(MPI)
    if (MPI_C_FOUND)
        message(STATUS "MPI found")
        set(GGML_HEADERS_MPI ggml-mpi.h)
        set(GGML_SOURCES_MPI ggml-mpi.c ggml-mpi.h)
        add_compile_definitions(GGML_USE_MPI)
        add_compile_definitions(${MPI_C_COMPILE_DEFINITIONS})
        if (NOT MSVC)
            add_compile_options(-Wno-cast-qual)
        endif()
        set(LLAMA_EXTRA_LIBS     ${LLAMA_EXTRA_LIBS}     ${MPI_C_LIBRARIES})
        set(LLAMA_EXTRA_INCLUDES ${LLAMA_EXTRA_INCLUDES} ${MPI_C_INCLUDE_DIRS})
        # Even if you're only using the C header, C++ programs may bring in MPI
        # C++ functions, so more linkage is needed
        if (MPI_CXX_FOUND)
            set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS}     ${MPI_CXX_LIBRARIES})
        endif()
    else()
        message(WARNING "MPI not found")
    endif()
endif()

if (LLAMA_CLBLAST)
    find_package(CLBlast)
    if (CLBlast_FOUND)
        message(STATUS "CLBlast found")

        set(GGML_HEADERS_OPENCL ggml-opencl.h)
        set(GGML_SOURCES_OPENCL ggml-opencl.cpp)

        add_compile_definitions(GGML_USE_CLBLAST)

        set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS} clblast)
    else()
        message(WARNING "CLBlast not found")
    endif()
endif()

if (LLAMA_HIPBLAS)
    list(APPEND CMAKE_PREFIX_PATH /opt/rocm)

    if (NOT ${CMAKE_C_COMPILER_ID} MATCHES "Clang")
        message(WARNING "Only LLVM is supported for HIP, hint: CC=/opt/rocm/llvm/bin/clang")
    endif()
    if (NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        message(WARNING "Only LLVM is supported for HIP, hint: CXX=/opt/rocm/llvm/bin/clang++")
    endif()

    find_package(hip)
    find_package(hipblas)
    find_package(rocblas)

    if (${hipblas_FOUND} AND ${hip_FOUND})
        message(STATUS "HIP and hipBLAS found")
        add_compile_definitions(GGML_USE_HIPBLAS GGML_USE_CUBLAS)
        add_library(ggml-rocm OBJECT ggml-cuda.cu ggml-cuda.h)
        if (BUILD_SHARED_LIBS)
            set_target_properties(ggml-rocm PROPERTIES POSITION_INDEPENDENT_CODE ON)
        endif()
        if (LLAMA_CUDA_FORCE_DMMV)
            target_compile_definitions(ggml-rocm PRIVATE GGML_CUDA_FORCE_DMMV)
        endif()
        target_compile_definitions(ggml-rocm PRIVATE GGML_CUDA_DMMV_X=${LLAMA_CUDA_DMMV_X})
        target_compile_definitions(ggml-rocm PRIVATE GGML_CUDA_MMV_Y=${LLAMA_CUDA_MMV_Y})
        target_compile_definitions(ggml-rocm PRIVATE K_QUANTS_PER_ITERATION=${LLAMA_CUDA_KQUANTS_ITER})
        set_source_files_properties(ggml-cuda.cu PROPERTIES LANGUAGE CXX)
        target_link_libraries(ggml-rocm PRIVATE hip::device PUBLIC hip::host roc::rocblas roc::hipblas)

        if (LLAMA_STATIC)
            message(FATAL_ERROR "Static linking not supported for HIP/ROCm")
        endif()
        set(LLAMA_EXTRA_LIBS ${LLAMA_EXTRA_LIBS} ggml-rocm)
    else()
        message(WARNING "hipBLAS or HIP not found. Try setting CMAKE_PREFIX_PATH=/opt/rocm")
    endif()
endif()

if (LLAMA_ALL_WARNINGS)
    if (NOT MSVC)
        set(warning_flags -Wall -Wextra -Wpedantic -Wcast-qual -Wno-unused-function)
        set(c_flags -Wshadow -Wstrict-prototypes -Wpointer-arith -Wmissing-prototypes -Werror=implicit-int -Werror=implicit-function-declaration)
        set(cxx_flags -Wmissing-declarations -Wmissing-noreturn)
        set(host_cxx_flags "")

        if (CMAKE_C_COMPILER_ID MATCHES "Clang")
            set(warning_flags ${warning_flags} -Wunreachable-code-break -Wunreachable-code-return)
            set(host_cxx_flags ${host_cxx_flags} -Wmissing-prototypes -Wextra-semi)

            if (
                (CMAKE_C_COMPILER_ID STREQUAL "Clang"      AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 3.8.0) OR
                (CMAKE_C_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 7.3.0)
            )
                set(c_flags ${c_flags} -Wdouble-promotion)
            endif()
        elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
            set(c_flags ${c_flags} -Wdouble-promotion)
            set(host_cxx_flags ${host_cxx_flags} -Wno-array-bounds)

            if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7.1.0)
                set(host_cxx_flags ${host_cxx_flags} -Wno-format-truncation)
            endif()
            if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.1.0)
                set(host_cxx_flags ${host_cxx_flags} -Wextra-semi)
            endif()
        endif()
    elseif(MSVC)    
        add_compile_options(
            $<$<CONFIG:>:/MT> #---------|
            $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
            $<$<CONFIG:Release>:/MT> #--|
        )
    else()
        # todo : msvc
    endif()

    set(c_flags   ${c_flags}   ${warning_flags})
    set(cxx_flags ${cxx_flags} ${warning_flags})
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:${c_flags}>"
                        "$<$<COMPILE_LANGUAGE:CXX>:${cxx_flags}>"
                        "$<$<COMPILE_LANGUAGE:CXX>:${host_cxx_flags}>")

endif()

if (NOT MSVC)
    set(cuda_flags -Wno-pedantic)
endif()
set(cuda_flags ${cxx_flags} -use_fast_math ${cuda_flags})

list(JOIN host_cxx_flags " " cuda_host_flags)  # pass host compiler flags as a single argument
if (NOT cuda_host_flags STREQUAL "")
    set(cuda_flags ${cuda_flags} -Xcompiler ${cuda_host_flags})
endif()

add_compile_options("$<$<COMPILE_LANGUAGE:CUDA>:${cuda_flags}>")

if (WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    if (BUILD_SHARED_LIBS)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
endif()

if (LLAMA_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if (result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO is not supported: ${output}")
    endif()
endif()







# Set the variables that need to be passed to the llamaCpp subproject
#set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
#set(LLAMA_CLBLAST ON CACHE BOOL "Enable CLBlast for llamaCpp" FORCE)
#set(LLAMA_CUBLAS ON CACHE BOOL "Enable Cuda for llamaCpp" FORCE)
#set(CMAKE_PREFIX_PATH "C:\\vcpkg\\packages\\clblast_x64-windows\\lib\\cmake\\CLBlast" CACHE STRING "Prefix path for CLBlast" FORCE)

# Include the subprojects
add_subdirectory(llamaCpp)
add_subdirectory(SocialEngine)
add_subdirectory(SocialEngineTest)
add_subdirectory(Run)
add_subdirectory(bertCpp)

# If you need to enable testing for your project
enable_testing()
