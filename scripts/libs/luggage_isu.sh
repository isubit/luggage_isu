#!/bin/bash

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory.
#
# git clone git@github.com:isubit/luggage_isu.git
# cd luggage_isu
# ./scripts/build_luggage_isu.sh
#


install_luggage_isu_suitcase() {
  # Install Theme - Suitcase
  drush en -y suitcase
  drush vset theme_default suitcase
  drush en -y suitcase_config
}

install_luggage_isu_suitcase_interim() {
  # Install Theme - Suitcase Interim
  drush en -y suitcase_interim
  drush vset theme_default suitcase_interim
  drush en -y suitcase_interim_frontpanel
}

install_luggage_isu_features() {
  # Install luggage_isu features
  drush en -y luggage_people_isu
  drush en -y isushib isushibsiteaccess
}
