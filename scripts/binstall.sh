#!/bin/sh

if [ ! $( basename $(pwd) ) = "nethacked" ]; then
  echo "Run from the root directory of nethacked"
  exit 1
fi

# Make sure we're up to date
tup || ( echo "Build Failed"; exit 1 )
(cd bin; rm -rf *)
cp src/nethacked bin/nethack
cp dat/*.lev dat/dungeon bin
cp sys/unix/nethack.sh bin
cp dat/quest.dat bin
cp dat/rumors bin

touch bin/perm bin/record bin/logfile bin/xlogfile
(
  cd bin
  sed -i '' -e "s;^HACKDIR=.*$;HACKDIR=$(pwd);" nethack.sh
  chmod a+x nethack.sh
)
