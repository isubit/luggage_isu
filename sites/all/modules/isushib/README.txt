
Shibboleth module
=================

DESCRIPTION
-----------
This module integrates shibboleth-based authentication with Drupal.

For more information about shibboleth, see http://shibboleth.org.

PREREQUISITES
-------------
Before using this module, make sure that your shibboleth environment
is set up and running correctly.

The best way to do this is to create a subdirectory on your webserver
with an HTML file and an .htaccess file in it. The .htaccess file
would look something like this:

AuthType shibboleth
ShibRequestSetting requireSession 1
Require shibboleth
ShibRedirectToSSL 443

Upon trying to access the HTML file, Apache should redirect you
to your shibboleth server, and upon authentication the shibboleth
server will redirect back to your HTML file. When all that is
working, you are ready to try this module.

INSTALLATION
------------
Place the entire isushib module folder into your third-party modules
directory, typically at sites/all/modules.

Enable the module in Drupal by going to Administer -> Site building ->
Modules.

You can configure shibboleth.module by going to Administer -> Site 
configuration -> Shibboleth but if you are running a standard shibboleth
setup there should be no configuration necessary; the defaults are fine.

Ensure that Clean URLs are enabled at Administer -> Site configuration ->
Clean URLs.

If desired, enable the shibboleth login block by going to Administer -> 
Site building -> Blocks.

SHIBBOLETH SITE ACCESS (optional)
---------------------------------
Shibboleth Site Access is a module that is bundled with the isushib
module. It allows you to restrict login to a whitelist of known usernames.
For example, if you are creating a site for example.edu, all example.edu
users may log in. By enabling the Shibboleth Site Access module, you
can restrict login to just users tom, dick, and harry by going to
Administration / Configuration/ People / Change who may access this site.

The permission to add/delete users from the whitelist is called "administer
shibboleth site access".

Perhaps a better name for the module would have been Shibboleth Access
Control List.

Roles can also be assigned to future users (i.e., users who are on the
whitelist but have not logged in yet) so that when they do log in they
are assigned a role or roles.

The permission to assign future roles to users on the whitelist is called
"administer shibboleth future roles".

Role assignment to users who have already logged in is possible if the person
assigning has 'administer users' and 'administer permissions' access; in other
words, the same permissions required to assign roles in Drupal core.

LDAP INTEGRATION
----------------
LDAP is integrated into shibboleth module. If you don't want to use LDAP,
leave the LDAP server setting blank.

To have user fields automatically populated by LDAP when users
register, name the Machine Name of the field to be the same
as the LDAP directory field name. For example, if your LDAP directory
returns a field called "displayname" containing the user's full name,
go to Administer -> Configuration -> People -> Account Settings and click
on the Manage Fields tab, then add a new field of type Text with the Machine
Name "field_displayname".

HOOKING INTO SHIBBOLETH USER ACCOUNT CREATION
---------------------------------------------
The Drupal hooks that fire during the login process are not helpful for
determining a first-time shibboleth-based authentication. So shibboleth provides
hook_shibboleth_post_register() in case you want your own module to do something
after the shibboleth account creation is complete. For example, we often use
that hook to match existing content that should be assigned to the user and
assign ownership programmatically, or create a new node that is the user's
profile when we don't want to use Drupal's user-based profiles.

TROUBLESHOOTING
---------------
If you see a 404 message "Shibboleth.sso" not found, you need to add the
following line to the .htaccess file in your Drupal site:

  # Pass all requests not referring directly to files in the filesystem to
  # index.php. Clean URLs are handled in drupal_environment_initialize().
  RewriteCond %{REQUEST_FILENAME} !-f
  RewriteCond %{REQUEST_FILENAME} !-d
  RewriteCond %{REQUEST_URI} !=/favicon.ico
  RewriteCond %{REQUEST_URI} !^/Shibboleth.sso(.+)$ <-- ADD THIS LINE
  RewriteRule ^ index.php [L]

WHY IT WORKS
------------
When you click on the Log In link provided by the shibboleth block, it takes
you to the directory you specified for "Login directory" under Administration /
Configuration / People / Shibboleth (by default, 'shibboleth'). The shibboleth 
module takes this path, adds "pc" (an arbitrary string) to the end of it and 
-- and here's the key -- registers it as a menu item in the menu hook. So now
http://yourdomain.com/shibboleth/pc is not a nonexistent file but a registered
Drupal path that is "located" inside a directory that's protected by a
.htaccess file restricting the contents to shibboleth-server-authenticated
users. So when you reach that path, the shibboleth module receives a call
to shibboleth_page() and goes from there.
