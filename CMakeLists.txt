CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(nethacked)

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/include)

LIST(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated-register")

# Base source files
ADD_LIBRARY(base
  src/alloc.cc
  src/decl.cc
  src/drawing.cc
  include/pm.h
  include/onames.h
  include/date.h)

ADD_EXECUTABLE(makedefs
  util/makedefs.cc
  entitylib/objects.cc
  entitylib/monst.cc)

FUNCTION(RUN_MAKEDEFS output opt)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${output}
    COMMAND makedefs -${opt} ${ARGN}
    WORKING_DIRECTORY src
    DEPENDS makedefs)
ENDFUNCTION()

ADD_EXECUTABLE(dlb
  util/dlb_main.cc)

# Executions for makedepends to generate appropriate files
RUN_MAKEDEFS(include/pm.h p)
RUN_MAKEDEFS(include/onames.h o)
RUN_MAKEDEFS(include/vis_tab.h y include/vis_tab.h)
RUN_MAKEDEFS(include/date.h u)
RUN_MAKEDEFS(src/monstr.cc m)
RUN_MAKEDEFS(src/vis_tab.cc z ../src/vis_tab.cc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

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

ADD_EXECUTABLE(dgn_comp
  compilers/dgn_parse.cc compilers/dgn_lex.cc compilers/dgn_main.cc)
TARGET_LINK_LIBRARIES(dgn_comp base)
ADD_EXECUTABLE(lev_comp
  compilers/lev_parse.cc
  compilers/lev_lex.cc
  compilers/lev_main.cc
  compilers/panic.cc
  entitylib/monst.cc
  entitylib/objects.cc
  include/pm.h
  include/onames.h
  include/date.h)

ADD_DEPENDENCIES(lev_comp
  include/pm.h
  include/onames.h
  include/date.h)

TARGET_LINK_LIBRARIES(lev_comp base)

ADD_EXECUTABLE(nethacked
  # Base Source
  src/allmain.cc
  src/alloc.cc
  src/apply.cc
  src/artifact.cc
  src/attrib.cc
  src/ball.cc
  src/bones.cc
  src/botl.cc
  src/cmd.cc
  src/dbridge.cc
  src/decl.cc
  src/detect.cc
  src/dig.cc
  src/display.cc
  src/dlb.cc
  src/do.cc
  src/do_name.cc
  src/do_wear.cc
  src/dog.cc
  src/dogmove.cc
  src/dokick.cc
  src/dothrow.cc
  src/drawing.cc
  src/dungeon.cc
  src/eat.cc
  src/end.cc
  src/engrave.cc
  src/exper.cc
  src/explode.cc
  src/extralev.cc
  src/files.cc
  src/fountain.cc
  src/hack.cc
  src/hacklib.cc
  src/invent.cc
  src/light.cc
  src/lock.cc
  src/mail.cc
  src/makemon.cc
  src/mapglyph.cc
  src/mcastu.cc
  src/mhitm.cc
  src/mhitu.cc
  src/minion.cc
  src/mklev.cc
  src/mkmap.cc
  src/mkmaze.cc
  src/mkobj.cc
  src/mkroom.cc
  src/mon.cc
  src/mondata.cc
  src/monmove.cc
  src/mplayer.cc
  src/mthrowu.cc
  src/muse.cc
  src/music.cc
  src/o_init.cc
  src/objnam.cc
  src/options.cc
  src/pager.cc
  src/pickup.cc
  src/pline.cc
  src/polyself.cc
  src/potion.cc
  src/pray.cc
  src/priest.cc
  src/quest.cc
  src/questpgr.cc
  src/read.cc
  src/rect.cc
  src/region.cc
  src/restore.cc
  src/rip.cc
  src/rnd.cc
  src/role.cc
  src/rumors.cc
  src/save.cc
  src/shk.cc
  src/shknam.cc
  src/sit.cc
  src/sounds.cc
  src/sp_lev.cc
  src/spell.cc
  src/steal.cc
  src/steed.cc
  src/teleport.cc
  src/timeout.cc
  src/topten.cc
  src/track.cc
  src/trap.cc
  src/u_init.cc
  src/uhitm.cc
  src/vault.cc
  src/version.cc
  src/vision.cc
  src/weapon.cc
  src/were.cc
  src/wield.cc
  src/windows.cc
  src/wizard.cc
  src/worm.cc
  src/worn.cc
  src/write.cc
  src/zap.cc

  entitylib/monst.cc
  entitylib/objects.cc

  win/tty/getline.cc
  win/tty/termcap.cc
  win/tty/topl.cc
  win/tty/wintty.cc

  sys/share/ioctl.cc
  sys/share/unixtty.cc

  sys/unix/unixmain.cc
  sys/unix/unixres.cc
  sys/unix/unixunix.cc
  
  # Generated Source
  src/monstr.cc
  src/vis_tab.cc

  # Generated Headers
  include/pm.h
  include/onames.h
  include/date.h)

TARGET_LINK_LIBRARIES(nethacked curses)
