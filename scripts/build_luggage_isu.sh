#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory of a recently cloned
#   luggage_isu. Example:
#
# git clone git@github.com:isubit/luggage_isu.git
# cd luggage_isu
# ./scripts/build_luggage_isu.sh
#
# To build with Suitcase Classic theme, use
# ./scripts/build_luggage_isu.sh -t suitcase
#

default_theme=suitcase_interim

while getopts ":t:" opt; do
  case $opt in
    t)
      if [ "$OPTARG" == "suitcase" ]; then
        default_theme=suitcase
      fi
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument, e.g., -t suitcase" >&2
      exit 1
      ;;
  esac
done

# Include luggage functions for building
. $(dirname $0)/libs/luggage.sh
# Include luggage_isu functions for building
. $(dirname $0)/libs/luggage_isu.sh

init
install_site
install_luggage_isu_${default_theme}
install_luggage_features
install_luggage_isu_features
finish
