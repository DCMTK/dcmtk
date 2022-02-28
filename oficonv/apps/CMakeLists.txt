# we need flex and bison to compile the applications in this directory.
# We don't use CMake's BISON_TARGET and FLEX_TARGET macros because we don't want "make clean" to remove the generated C++ files.
find_package(BISON)
find_package(FLEX)

# declare include directories
include_directories("${oficonv_SOURCE_DIR}/apps" "${oficonv_SOURCE_DIR}/libsrc")

# definitions for mkcsmapper
DCMTK_ADD_EXECUTABLE(mkcsmapper ${oficonv_SOURCE_DIR}/apps/mkcsmapper_bison.cc ${oficonv_SOURCE_DIR}/apps/mkcsmapper_flex.cc)
DCMTK_TARGET_LINK_MODULES(mkcsmapper oficonv)

# definitions for mkesdb
DCMTK_ADD_EXECUTABLE(mkesdb ${oficonv_SOURCE_DIR}/apps/mkesdb_bison.cc ${oficonv_SOURCE_DIR}/apps/mkesdb_flex.cc)
DCMTK_TARGET_LINK_MODULES(mkesdb oficonv)

# Custom target "run_flex_and_bison" will rebuild the C++ files using flex and bison.
if (FLEX_FOUND AND BISON_FOUND)
    ADD_CUSTOM_TARGET(
        run_flex_and_bison
        COMMAND 
          ${FLEX_EXECUTABLE} --outfile=${oficonv_SOURCE_DIR}/apps/mkcsmapper_flex.cc ${CMAKE_CURRENT_SOURCE_DIR}/mkcsmapper.l
        COMMAND 
          ${FLEX_EXECUTABLE} --outfile=${oficonv_SOURCE_DIR}/apps/mkesdb_flex.cc ${CMAKE_CURRENT_SOURCE_DIR}/mkesdb.l
        COMMAND
          ${BISON_EXECUTABLE} --defines=${oficonv_SOURCE_DIR}/apps/mkcsmapper_bison.h --output=${oficonv_SOURCE_DIR}/apps/mkcsmapper_bison.cc ${CMAKE_CURRENT_SOURCE_DIR}/mkcsmapper.y
        COMMAND
          ${BISON_EXECUTABLE} --defines=${oficonv_SOURCE_DIR}/apps/mkesdb_bison.h --output=${oficonv_SOURCE_DIR}/apps/mkesdb_bison.cc ${CMAKE_CURRENT_SOURCE_DIR}/mkesdb.y
        COMMENT "Re-generating C++ files using flex and bison"
    )
endif()
