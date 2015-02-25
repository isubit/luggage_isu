#!/bin/sh

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory of luggage_isu.
#
# Example:
#
# cd luggage_isu (cd %your_site)
# ./scripts/pubcookie_to_shibboleth.sh
#

rm -rf sites/all/modules/luggage/luggage_pubcookie
git rm -rf sites/all/modules/pubcookie
rm -rf sites/all/modules/pubcookie
drush en isushib isushibsiteaccess -y
git submodule update --init --force