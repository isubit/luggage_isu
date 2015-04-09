#!/bin/bash

# Assumptions:
# ===========
# - Drush 5.9 or greater
# - Git 1.8.5.2 or greater
# - Execution from a shell in the webroot directory of luggage_isu.
#
# Example:
#
# cd /var/www/html/www.example.edu/my_luggage_isu_site
# ./scripts/pubcookie_to_shibboleth.sh
#
# You must pass the URI to drush if your settings are not in sites/default.
# Example if your settings file is in sites/www.example.edu/settings.php
# instead of sites/default/settings.php
#
# cd /var/www/html/www.example.edu/my_luggage_isu_site
# ./scripts/pubcookie_to_shibboleth.sh --uri=http://www.example.edu/
#

echo "You are about to modify a database."
echo
echo "Please make sure that you have a backup of the database before proceeding."
echo
echo
read -p "Proceed? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]
then
  # Remove the old Luggage Pubcookie feature
  rm -rf sites/all/modules/luggage/luggage_pubcookie
  
  # Remove the pubcookie module from the site-specific git repo's
  # working tree and index.
  git rm -rf sites/all/modules/pubcookie
  
  # Remove any untracked files
  rm -rf sites/all/modules/pubcookie
  
  # Enable isushib and isushibsiteaccess modules
  drush --simulate=0 en isushib isushibsiteaccess -y $1
  git submodule update --init --force
   
  
  # Move pubcookie data into new tables
  pubcookiesiteaccess_users=$(drush --simulate=0 sql-query "SELECT * FROM pubcookiesiteaccess_users LIMIT 1" $1)
  if [ "$pubcookiesiteaccess_users" == "" ]; then
    echo "Could not SELECT pubcookiesiteaccess_users (there may not have been any users defined); skipping import"
  else
    echo "Importing"
    drush -v --simulate=0 sql-query "INSERT isushibsiteaccess_users SELECT * FROM pubcookiesiteaccess_users;" -y $1
  fi
  drush -v --simulate=0 sql-query "DROP TABLE pubcookiesiteaccess_users;" -y $1
  
  pubcookiesiteaccess_roles=$(drush --simulate=0 sql-query "SELECT * FROM pubcookiesiteaccess_roles LIMIT 1" $1)
  if [ "$pubcookiesiteaccess_roles" == "" ]; then
    echo "Could not SELECT pubcookiesiteaccess_roles (there may not have been any roles defined); skipping import"
  else
    echo "importing roles"
    drush -v --simulate=0 sql-query "INSERT isushibsiteaccess_roles SELECT * FROM pubcookiesiteaccess_roles;" -y $1
  fi
  drush -v --simulate=0 sql-query "DROP TABLE pubcookiesiteaccess_roles;" -y $1
  
  # Migrate permissions
  drush -v --simulate=0 sql-query "UPDATE role SET name = 'shibboleth user admin' WHERE name = 'pubcookie user admin';" -y $1
  drush -v --simulate=0 sql-query "DELETE FROM role_permission WHERE module = 'pubcookiesiteaccess';" -y $1
  drush -v --simulate=0 sql-query "UPDATE role_permission SET rid = (SELECT rid FROM role WHERE name = 'shibboleth user admin') WHERE module = 'isushibsiteaccess';" -y $1
  
  # Migrate assignable roles
  assignableroles=$(drush --simulate=0 sql-query "SELECT name FROM variable WHERE name = 'isushibsiteaccess_assignable_roles'" -y $1)
  if [ "$assignableroles" == "" ]; then
    echo "migrating assignable roles"
    drush -v --simulate=0 sql-query "UPDATE variable SET name = 'isushibsiteaccess_assignable_roles' WHERE name = 'pubcookiesiteaccess_assignable_roles' LIMIT 1;" -y $1
  fi
  
  # Clean up any lingering pubcookie variables
  drush --simulate=0 vdel pubcookie_domain -y $1
  drush --simulate=0 vdel pubcookie_id_is_email -y $1
  drush --simulate=0 vdel pubcookie_ldap_basedn -y $1 
  drush --simulate=0 vdel pubcookie_ldap_searchfield -y $1
  drush --simulate=0 vdel pubcookie_ldap_server -y $1
  drush --simulate=0 vdel pubcookie_ldap_usernamefield -y $1
  drush --simulate=0 vdel pubcookie_login_dir -y $1
  drush --simulate=0 vdel pubcookie_success_url -y $1
  
  # clear cache for good measure
  drush --simulate=0 cc all $1
  
  echo "Completed. Do a git status. Your next step will probably be"
  echo "git commit -m 'Removed pubcookie module'"
  echo
  echo "Note that if a login directory exists it needs to be removed."
  echo
  exit
fi

echo "Aborted."
