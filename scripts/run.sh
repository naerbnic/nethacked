#!/bin/sh
scripts/binstall.sh || exit 1
echo '==STARTING==' >> err.txt
bin/nethack.sh 2>>err.txt
