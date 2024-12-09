# Expects a parameter "PASSPHRASE" with which to encrypt the archive

find_program(mktemp NAMES mktemp)
find_program(pwsh NAMES pwsh)

if(mktemp)
    execute_process(
        COMMAND mktemp
        RESULT_VARIABLE ret
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE TEMP_ARCHIVE
    )
    if(NOT ret EQUAL 0)
        message(FATAL_ERROR "Failed to create a temp file: ${out}")
    endif()
elseif(pwsh)
    execute_process(
        COMMAND pwsh -c "New-TemporaryFile | Select -ExpandProperty FullName"
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE TEMP_ARCHIVE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT ret EQUAL 0)
        message(FATAL_ERROR "Failed to create a temp file: (${ret}) ${out}")
    endif()
else()
    message(FATAL_ERROR "Could not find mktemp or pwsh to make temporary directory")
endif()

file(GLOB_RECURSE INPUTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" *.input)

# Ignore input files in build directories
list(FILTER INPUTS EXCLUDE REGEX "build(-vsc)?/")

file(
    ARCHIVE_CREATE
    OUTPUT "${TEMP_ARCHIVE}"
    FORMAT zip
    PATHS ${INPUTS}
)

execute_process(
    COMMAND gpg --symmetric --cipher-algo=AES256 --passphrase=${PASSPHRASE} -o "${CMAKE_CURRENT_SOURCE_DIR}/inputs.zip.gpg" --batch --yes "${TEMP_ARCHIVE}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    RESULT_VARIABLE ret
)

file(REMOVE "${TEMP_ARCHIVE}")
