for header in $(ls include); do
  scripts/xfiles sed -i '' -e "s/^#include \"$(basename ${header})\"/#include \"core\/$(basename ${header})\"/g"
done
