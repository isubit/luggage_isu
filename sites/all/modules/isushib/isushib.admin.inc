<?php

/**
 * @file
 * Admin page callbacks for the isushib module.
 */

/*
 * The shibboleth settings page.
 */
function isushib_settings() {
  global $base_url; // http://www.example.edu/drupal
  $array = explode('/', $base_url);
  $base_domain = $array[2]; // www.example.edu
  $parts = explode('.', $base_domain);

  if (count($parts) == 1) {
    drupal_set_message(t('Shibboleth only works on real domains, e.g., example.edu.'));
    $domain = array_pop($parts);
  }
  else { // Parse out example.edu.
    //        'example'                   '.'   'edu'
    $domain = $parts[count($parts) - 2] . '.' . $parts[count($parts) -1];
  }

  $form['isushib_login_dir'] = array(
    '#type' => 'textfield',
    '#title' => t('Login directory'),
    '#default_value' => variable_get('isushib_login_dir', 'shibboleth'),
    '#description' => t('What is the subdirectory in this Drupal installation that contains the .htaccess file with the AuthType shibboleth directive? (Do not use a trailing slash.) The default is "shibboleth".'),
    '#size' => '40',
    '#maxlength' => '255'
    );
  $form['isushib_success_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Successful login URL'),
    '#default_value' => variable_get('isushib_success_url', ''),
    '#description' => t('Where do you want the user directed after a successful shibboleth login? Leave blank if you want the user to return to the page that they were on when they logged in. The default is to leave this blank.'),
    '#size' => '40',
    '#maxlength' => '255'
  );

  return system_settings_form($form);
}
