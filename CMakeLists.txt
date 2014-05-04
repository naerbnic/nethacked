CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(nethacked)

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/include)

LIST(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated-register")

# Base source files
ADD_LIBRARY(base src/alloc.cc src/decl.cc src/drawing.cc)

ADD_EXECUTABLE(makedefs
  util/makedefs.cc
  entitylib/objects.cc
  entitylib/monst.cc)

FUNCTION(RUN_MAKEDEFS output opt)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${output}
    COMMAND makedefs -${opt} ${ARGN}
    DEPENDS makedefs)
ENDFUNCTION()

ADD_EXECUTABLE(dlb
  util/dlb_main.cc)

# Executions for makedepends to generate appropriate files
RUN_MAKEDEFS(include/pm.h p)
RUN_MAKEDEFS(include/onames.h o)
RUN_MAKEDEFS(include/vis_tab.h y include/vis_tab.h)
RUN_MAKEDEFS(include/date.h u)

FIND_PACKAGE(BISON)

IF(BISON_FOUND)
  FUNCTION(ADD_PARSER y_file c_file h_file)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${c_file} ${h_file}
      COMMAND ${BISON_EXECUTABLE} -d -o ${c_file} ${y_file}
      MAIN_DEPENDENCY ${y_file}
      )
  ENDFUNCTION()
ENDIF()

FIND_PACKAGE(FLEX)

IF(FLEX_FOUND)
  FUNCTION(ADD_LEXER l_file c_file h_file)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${c_file} ${h_file}
      COMMAND ${FLEX_EXECUTABLE} -o ${c_file} "--header-file=${h_file}" ${l_file}
      MAIN_DEPENDENCY ${l_file}
      )
  ENDFUNCTION()
ENDIF()

ADD_PARSER(compilers/dgn_parse.y compilers/dgn_parse.cc compilers/dgn_parse.hh)
ADD_PARSER(compilers/lev_parse.y compilers/lev_parse.cc compilers/lev_parse.hh)
ADD_LEXER(compilers/dgn_lex.l compilers/dgn_lex.cc compilers/dgn_lex.h)
ADD_LEXER(compilers/lev_lex.l compilers/lev_lex.cc compilers/lev_lex.h)

ADD_EXECUTABLE(dgn_comp compilers/dgn_parse.cc compilers/dgn_lex.cc compilers/dgn_main.cc)
TARGET_LINK_LIBRARIES(dgn_comp base)
ADD_EXECUTABLE(
  lev_comp
  compilers/lev_parse.cc
  compilers/lev_lex.cc
  compilers/lev_main.cc
  compilers/panic.cc
  entitylib/monst.cc
  entitylib/objects.cc
  include/pm.h
  include/onames.h
  include/date.h)

ADD_DEPENDENCIES(
  lev_comp
  include/pm.h
  include/onames.h
  include/date.h)

TARGET_LINK_LIBRARIES(lev_comp base)

