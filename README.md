Luggage_ISU
=========
[![Build Status](https://travis-ci.org/isubit/luggage_isu.svg?branch=master "Build Status")](http://travis-ci.org/isubit/luggage_isu)

Luggage_ISU is a downstream fork of [Luggage](https://github.com/isubit/luggage) with additions specifically designed for use at Iowa State University.

Luggage_ISU adds the following features to Luggage:
* Support for Iowa State University's single signon system
* People profiles are automatically populated from ISU's LDAP directory
* Iowa State University look and feel

Version
----
Development of Luggage_ISU takes place on the 'development' branch in Github. Pull requests should be made against the 'development' branch.

Releases take place on the 'master' branch. 

The 2.x.x branch of Luggage_ISU is no longer supported.

Installation
--------------

Below is the brief run-down on how to install Luggage_ISU. The full installation documentation can be found [here](http://luggagedocs.info/installing-luggage-scratch).

#####Assumptions:
* Drush is installed
* Git is installed

Clone this repo into the root directory that your web server points to.

Run the build script found within the *scripts* directory:
```
bash scripts/build_luggage_isu.sh
```

The last line of output from the script, if successful, will be a root user one-time login link. Copy and append everything from "/user" and on, to the end of the luggage installation url in a web browser of your choice. Typically, the final link will look something like this:
``` 
http://localhost/luggage/user/reset/1/1409061179/KjbHsr6O7FRaz-__WShbgWuPwKHKrXHy6QGV_AQu02E/login
```

You are now logged in as the root user, allowing you to develop using the power of Luggage! Enjoy!

Contributing
----

Refer to the [Luggage git architecture](http://luggagedocs.info/comprehensive-code-flow-management)

Refer to the [contribution documentation](http://luggagedocs.info/luggage-development)

Troubleshooting
----

Read/Search [Luggage Documentation][]

Join us on IRC FreeNode @ #luggage
* Need help setting up IRC? https://www.drupal.org/irc/setting-up

Travis-CI - https://travis-ci.org/isubit/luggage_isu
* Reports into IRC FreeNode @  ##luggage


License
----

[GPLv2][]


**Open Source | Open Access | Open Mind**

[Drupal]:http://drupal.org/
[Drush]:https://github.com/drush-ops/drush
[Ckeditor]:http://ckeditor.com/
[Flexslider2]:http://flexslider.woothemes.com/
[Apache Solr]:http://lucene.apache.org/solr/
[Shibboleth]:https://shibboleth.net/
[GPLv2]:http://www.gnu.org/licenses/gpl-2.0.html
[Travis]:https://travis-ci.org/isubit/luggage.svg?branch=master
[Luggage ISU]:https://github.com/isubit/luggage_isu
[Luggage Documentation]:http://luggagedocs.info/
