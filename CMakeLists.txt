CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(nethacked)

INCLUDE_DIRECTORIES(BEFORE ${CMAKE_SOURCE_DIR}/include)

LIST(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wno-deprecated-register")

ADD_EXECUTABLE(makedefs
  util/makedefs.cc
  cxx/buffer.cc
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

RUN_MAKEDEFS(dat/data d)
RUN_MAKEDEFS(dat/rumors r)
RUN_MAKEDEFS(dat/quest.dat q)
RUN_MAKEDEFS(dat/oracles h)
RUN_MAKEDEFS(dat/options v)
RUN_MAKEDEFS(dat/dungeon.pdf e)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/cxx)

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
  src/alloc.cc
  src/decl.cc
  src/drawing.cc
  cxx/buffer.cc
  include/pm.h
  include/onames.h
  include/date.h)

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
  src/alloc.cc
  src/decl.cc
  src/drawing.cc

  include/pm.h
  include/onames.h
  include/date.h)

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

FIND_PACKAGE(CURSES)
TARGET_LINK_LIBRARIES(nethacked ${CURSES_LIBRARIES})


