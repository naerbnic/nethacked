CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(nethacked)

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/src)

# Configure JSON Sprit.
# Right now, this is manual.

FIND_PATH(JSON_SPIRIT_INCLUDE_DIR
  NAMES json_spirit.h)

FIND_LIBRARY(JSON_SPIRIT_LIBRARY NAMES json_spirit)

IF(JSON_SPIRIT_INCLUDE_DIR AND JSON_SPIRIT_LIBRARY)
  SET(JSON_SPIRIT_FOUND TRUE)
  SET(JSON_SPIRIT_LIBRARIES ${JSON_SPIRIT_LIBRARY})
  SET(JSON_SPIRIT_INCLUDE_DIRS ${JSON_SPIRIT_INCLUDE_DIR})
ENDIF()

IF(JSON_SPIRIT_FOUND)
  INCLUDE_DIRECTORIES(${JSON_SPIRIT_INCLUDE_DIRS})
ENDIF()

LIST(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated-register -Wno-format-security -Wno-writable-strings")

ADD_EXECUTABLE(src/cxx_test
  src/cxx/process.cc
  src/cxx/exception.cc
  src/cxx/stacktrace.cc
  src/cxx/buffer.cc
  src/cxx/json_helpers.cc
  src/cxx/test_main.cc)
TARGET_LINK_LIBRARIES(src/cxx_test ${JSON_SPIRIT_LIBRARIES})

ADD_EXECUTABLE(makedefs
  util/makedefs.cc
  src/cxx/buffer.cc
  src/cxx/json_helpers.cc
  entitylib/objects.cc
  entitylib/monst.cc)

TARGET_LINK_LIBRARIES(makedefs ${JSON_SPIRIT_LIBRARIES})

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
RUN_MAKEDEFS(src/core/pm.h p)
RUN_MAKEDEFS(src/core/onames.h o)
RUN_MAKEDEFS(src/core/vis_tab.h y src/core/vis_tab.h)
RUN_MAKEDEFS(src/core/date.h u)

RUN_MAKEDEFS(src/core/monstr.cc m)
RUN_MAKEDEFS(src/core/vis_tab.cc z ../src/core/vis_tab.cc)

RUN_MAKEDEFS(dat/data d)
RUN_MAKEDEFS(dat/rumors r)
RUN_MAKEDEFS(dat/quest.dat q)
RUN_MAKEDEFS(dat/oracles h)
RUN_MAKEDEFS(dat/options v)
RUN_MAKEDEFS(dat/dungeon.pdf e)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src)

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
  compilers/dgn_parse.cc
  compilers/dgn_lex.cc
  compilers/dgn_main.cc
  src/core/alloc.cc
  src/core/decl.cc
  src/core/drawing.cc
  src/cxx/buffer.cc
  src/cxx/json_helpers.cc
  src/core/pm.h
  src/core/onames.h
  src/core/date.h)

ADD_CUSTOM_COMMAND(
  OUTPUT dat/dungeon
  COMMAND dgn_comp dungeon.pdf
  WORKING_DIRECTORY dat
  DEPENDS dgn_comp dat/dungeon.pdf)

FUNCTION(COMPILE_LEVEL des_file file_var)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${ARGN}
    COMMAND lev_comp ${CMAKE_SOURCE_DIR}/${des_file}
    WORKING_DIRECTORY dat
    DEPENDS lev_comp)
  SET(${file_var} ${ARGN} PARENT_SCOPE)
ENDFUNCTION()

# Special Levels
COMPILE_LEVEL(dat/bigroom.des BIGROOM_FILES
  dat/bigrm-1.lev
  dat/bigrm-2.lev
  dat/bigrm-3.lev
  dat/bigrm-4.lev
  dat/bigrm-5.lev)


COMPILE_LEVEL(dat/castle.des CASTLE_FILES
  dat/castle.lev)

COMPILE_LEVEL(dat/endgame.des ENDGAME_FILES
  dat/earth.lev
  dat/air.lev
  dat/water.lev
  dat/fire.lev
  dat/astral.lev)

COMPILE_LEVEL(dat/gehennom.des GEHENNOM_FILES
  dat/asmodeus.lev
  dat/baalz.lev
  dat/juiblex.lev
  dat/orcus.lev
  dat/sanctum.lev
  dat/valley.lev)

COMPILE_LEVEL(dat/knox.des KNOX_FILES
  dat/knox.lev)

COMPILE_LEVEL(dat/medusa.des MEDUSA_FILES
  dat/medusa-1.lev
  dat/medusa-2.lev)

COMPILE_LEVEL(dat/mines.des MINES_FILES
  dat/minefill.lev
  dat/minetn-1.lev
  dat/minetn-2.lev
  dat/minetn-3.lev
  dat/minetn-4.lev
  dat/minetn-5.lev
  dat/minetn-6.lev
  dat/minetn-7.lev
  dat/minend-1.lev
  dat/minend-2.lev
  dat/minend-3.lev)

COMPILE_LEVEL(dat/oracle.des ORACLE_FILES
  dat/oracle.lev)

COMPILE_LEVEL(dat/sokoban.des SOKOBAN_FILES
  dat/soko1-1.lev
  dat/soko1-2.lev
  dat/soko2-1.lev
  dat/soko2-2.lev
  dat/soko3-1.lev
  dat/soko3-2.lev
  dat/soko4-1.lev
  dat/soko4-2.lev)

COMPILE_LEVEL(dat/tower.des TOWER_FILES
  dat/tower1.lev
  dat/tower2.lev
  dat/tower3.lev)

COMPILE_LEVEL(dat/yendor.des YENDOR_FILES
  dat/fakewiz1.lev
  dat/fakewiz2.lev
  dat/wizard1.lev
  dat/wizard2.lev
  dat/wizard3.lev)

FUNCTION(COMPILE_QUEST_LEVEL base_name prefix file_var)
  COMPILE_LEVEL(dat/${base_name}.des FILES
    dat/${prefix}-strt.lev
    dat/${prefix}-loca.lev
    dat/${prefix}-goal.lev
    dat/${prefix}-fila.lev
    dat/${prefix}-filb.lev)
  SET(${file_var} ${FILES} PARENT_SCOPE)
ENDFUNCTION()

COMPILE_QUEST_LEVEL(Arch Arc ARC_QUEST_FILES)
COMPILE_QUEST_LEVEL(Barb Bar BAR_QUEST_FILES)
COMPILE_QUEST_LEVEL(Caveman Cav CAV_QUEST_FILES)
COMPILE_QUEST_LEVEL(Healer Hea HEA_QUEST_FILES)
COMPILE_QUEST_LEVEL(Knight Kni KNI_QUEST_FILES)
COMPILE_QUEST_LEVEL(Monk Mon MON_QUEST_FILES)
COMPILE_QUEST_LEVEL(Priest Pri PRI_QUEST_FILES)
COMPILE_QUEST_LEVEL(Ranger Ran RAN_QUEST_FILES)
COMPILE_QUEST_LEVEL(Rogue Rog ROG_QUEST_FILES)
COMPILE_QUEST_LEVEL(Samurai Sam SAM_QUEST_FILES)
COMPILE_QUEST_LEVEL(Tourist Tou TOU_QUEST_FILES)
COMPILE_QUEST_LEVEL(Valkyrie Val VAL_QUEST_FILES)
COMPILE_QUEST_LEVEL(Wizard Wiz WIZ_QUEST_FILES)

SET(DATA_FILES
  dat/data
  dat/rumors
  dat/quest.dat
  dat/oracles
  dat/options

  dat/dungeon
   
  ${BIGROOM_FILES}
  ${CASTLE_FILES}
  ${ENDGAME_FILES}
  ${GEHENNOM_FILES}
  ${KNOX_FILES}
  ${MEDUSA_FILES}
  ${MINES_FILES}
  ${ORACLE_FILES}
  ${SOKOBAN_FILES}
  ${TOWER_FILES}
  ${YENDOR_FILES}
  
  ${ARC_QUEST_FILES}
  ${BAR_QUEST_FILES}
  ${CAV_QUEST_FILES}
  ${HEA_QUEST_FILES}
  ${KNI_QUEST_FILES}
  ${MON_QUEST_FILES}
  ${PRI_QUEST_FILES}
  ${RAN_QUEST_FILES}
  ${ROG_QUEST_FILES}
  ${SAM_QUEST_FILES}
  ${TOU_QUEST_FILES}
  ${VAL_QUEST_FILES}
  ${WIZ_QUEST_FILES})

ADD_CUSTOM_TARGET(dat
  ALL
  DEPENDS ${DATA_FILES})

ADD_EXECUTABLE(lev_comp
  compilers/lev_parse.cc
  compilers/lev_lex.cc
  compilers/lev_main.cc
  compilers/panic.cc
  entitylib/monst.cc
  entitylib/objects.cc
  src/core/alloc.cc
  src/core/decl.cc
  src/core/drawing.cc

  src/core/pm.h
  src/core/onames.h
  src/core/date.h)

ADD_EXECUTABLE(nethacked
  # Base Source
  src/core/allmain.cc
  src/core/alloc.cc
  src/core/apply.cc
  src/core/artifact.cc
  src/core/attrib.cc
  src/core/ball.cc
  src/core/bones.cc
  src/core/botl.cc
  src/core/cmd.cc
  src/core/dbridge.cc
  src/core/decl.cc
  src/core/detect.cc
  src/core/dig.cc
  src/core/display.cc
  src/core/dlb.cc
  src/core/do.cc
  src/core/do_name.cc
  src/core/do_wear.cc
  src/core/dog.cc
  src/core/dogmove.cc
  src/core/dokick.cc
  src/core/dothrow.cc
  src/core/drawing.cc
  src/core/dungeon.cc
  src/core/eat.cc
  src/core/end.cc
  src/core/engrave.cc
  src/core/exper.cc
  src/core/explode.cc
  src/core/extralev.cc
  src/core/files.cc
  src/core/fountain.cc
  src/core/hack.cc
  src/core/hacklib.cc
  src/core/invent.cc
  src/core/light.cc
  src/core/lock.cc
  src/core/mail.cc
  src/core/makemon.cc
  src/core/mapglyph.cc
  src/core/mcastu.cc
  src/core/mhitm.cc
  src/core/mhitu.cc
  src/core/minion.cc
  src/core/mklev.cc
  src/core/mkmap.cc
  src/core/mkmaze.cc
  src/core/mkobj.cc
  src/core/mkroom.cc
  src/core/mon.cc
  src/core/mondata.cc
  src/core/monmove.cc
  src/core/mplayer.cc
  src/core/mthrowu.cc
  src/core/muse.cc
  src/core/music.cc
  src/core/o_init.cc
  src/core/objnam.cc
  src/core/options.cc
  src/core/pager.cc
  src/core/pickup.cc
  src/core/pline.cc
  src/core/polyself.cc
  src/core/potion.cc
  src/core/pray.cc
  src/core/priest.cc
  src/core/quest.cc
  src/core/questpgr.cc
  src/core/read.cc
  src/core/rect.cc
  src/core/region.cc
  src/core/restore.cc
  src/core/rip.cc
  src/core/rnd.cc
  src/core/role.cc
  src/core/rumors.cc
  src/core/save.cc
  src/core/shk.cc
  src/core/shknam.cc
  src/core/sit.cc
  src/core/sounds.cc
  src/core/sp_lev.cc
  src/core/spell.cc
  src/core/steal.cc
  src/core/steed.cc
  src/core/teleport.cc
  src/core/timeout.cc
  src/core/topten.cc
  src/core/track.cc
  src/core/trap.cc
  src/core/u_init.cc
  src/core/uhitm.cc
  src/core/vault.cc
  src/core/version.cc
  src/core/vision.cc
  src/core/weapon.cc
  src/core/were.cc
  src/core/wield.cc
  src/core/windows.cc
  src/core/wizard.cc
  src/core/worm.cc
  src/core/worn.cc
  src/core/write.cc
  src/core/zap.cc

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
  src/core/monstr.cc
  src/core/vis_tab.cc

  # Generated Headers
  src/core/pm.h
  src/core/onames.h
  src/core/date.h)

FIND_PACKAGE(CURSES)
TARGET_LINK_LIBRARIES(nethacked ${CURSES_LIBRARIES} ${JSON_SPIRIT_LIBRARIES})


