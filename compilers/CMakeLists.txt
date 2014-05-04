FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

SET(DungeonParserSource ${CMAKE_SOURCE_DIR}/compilers/dgn_yacc.cc)
SET(DungeonParserHeader ${CMAKE_SOURCE_DIR}/compilers/dgn_yacc.hh)
IF(BISON_FOUND)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${DungeonParserSource} ${DungeonParserHeader}
    COMMAND ${BISON_EXECUTABLE} -d -o
    )
ENDIF()
