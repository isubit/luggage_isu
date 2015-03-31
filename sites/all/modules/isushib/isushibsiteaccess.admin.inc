<?php

/**
 * @file
 * Administrative forms for shibboleth site access module.
 */

/*
 * The shibboleth site access settings page.
 */
function isushibsiteaccess_settings() {
  // Table of users who are already allowed to access the site and have accounts
  // along with future users who do not have accounts but may have roles 
  // preassigned.
  $header = array(
    array('data' => t('User'), 'field' => 'name', 'sort' => 'asc'), 
    t('Account Status'), 
    t('Roles'),
    t('Operations')
  );
  $rows = array();
  
  // A join on the users table gets us all users, current and future.
  $query = db_select('isushibsiteaccess_users', 'pu');
  $query->leftjoin('users', 'u', 'pu.name = u.name');
  $query->fields('pu', array('fuid', 'name'));
  
  $table_sort = $query->extend('TableSort')->orderByHeader($header);
  $pager = $table_sort->extend('PagerDefault')->limit(25);
  $result = $pager->execute();
  
  // The subset of all Drupal roles that may be assigned to future users.
  $assignable_roles = variable_get('isushibsiteaccess_assignable_roles', array());
  foreach ($result as $row) {
    // If user has not yet logged in $u will be false.
    $u = user_load_by_name($row->name);
    
    $assigned_roles = check_plain(implode(isushibsiteaccess_assigned_roles($row->fuid), ', '));
        
    // Operations links.
    $operations = '';
    if (user_access('administer shibboleth future roles')) {
      $operations .= l(t('edit'), "admin/config/people/isushibsiteaccess/edit/$row->fuid");
      $operations .= '&nbsp;&nbsp;';
    }
    $operations .= l(t('delete'), "admin/config/people/isushibsiteaccess/delete/$row->fuid");
    
    $edit_user = '';
    if ($u && user_access('administer users') && user_access('administer permissions')) {
      $edit_user = l(t('edit'), "user/$u->uid/edit", array('query' => array('destination' => 'admin/config/people/isushibsiteaccess')));
    }
    
    $rows[] = array(
      'data' => array(
        check_plain($row->name),
        $u ? t('Current') : t('Future'),
        $u ? check_plain(implode(', ', $u->roles)) : $assigned_roles,
        $u ? $edit_user : $operations,
      ),
    );
  }
  
  $form['isushibsiteaccess_list'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No users yet.')
  );
  $form['pager'] = array(
    '#theme' => 'pager',
  );
  
  // Add form for new users.
  $form['future'] = array(
    '#type' => 'fieldset',
    '#title' => t('Future Users'),
  );
  $form['future']['isushibsiteaccess_usernames'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#title' => t('Username(s) of future user(s)'),
    '#description' => t('Enter the username of a person who has not yet created an account but when they do, should be assigned a role. You may also provide multiple usernames delimited by commas (e.g., john, michael, ryan) or on multiple lines.')
  );
  
  $available_roles = isushibsiteaccess_available_roles();
  if ($available_roles && user_access('administer shibboleth future roles')) {
    $form['future']['future_user_roles'] = array(
      '#title' => t('Roles to be assigned at login'),
      '#type' => 'checkboxes',
      '#options' => $available_roles,
      '#description' => t('Select which roles should be assigned at login.'),
    );
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Validate user list.
 */
function isushibsiteaccess_settings_validate($form, &$form_state) {
  $users = array();
  
  // Change to an array.
  $lines = explode("\n", $form_state['values']['isushibsiteaccess_usernames']);
  foreach ($lines as $line) {
    $users = array_merge($users, explode(",", $line));   
  }
  
  // Zap whitespace.
  $trimmed_users = array_map('trim', $users);

  // Zap empty lines.
  $clean_users = array_filter($trimmed_users);

  // Validate names; e.g., j(**~±z is a bad name.
  foreach ($clean_users as $name) {
    // If user_validate_name() returns anything it's a bad sign.
    if (user_validate_name($name)) {
      form_set_error('isushibsiteaccess_usernames', t("'%name' is not a valid username.", array('%name' => $name)));
    }
  }

  $form_state['values']['clean_users'] = $clean_users;
}

/**
 * Process form submission.
 *
 * @see isushibsiteaccess_settings
 */
function isushibsiteaccess_settings_submit($form, &$form_state) {
  $single_user_being_added = count($form_state['values']['clean_users']) == 1;
  $count = 0;
  $cannot_assign = array();
  foreach ($form_state['values']['clean_users'] as $name) {
    // Does user already exist in Drupal?
    $u = user_load_by_name($name);
    $status = $u ? t('current') : t('future');
    
    // Does the user already exist as a future user?
    $fuid = isushibsiteaccess_get_fuid($name);
    if ($fuid) {
      form_set_error('isushibsiteaccess_existing_user', t('User %name has already been added as a @status user; skipping this user.', array('%name' => $name, '@status' => $status)));
      continue;
    }
    
    // Add user record.
    $record = array(
      'name' => $name,
    );
    drupal_write_record('isushibsiteaccess_users', $record);
    $status = $u ? t('current') : t('future');
    if ($single_user_being_added) {
      drupal_set_message(t('Added %status user %name.', array('%status' => $status, '%name' => $name)));
    }
    else {
      $count++;
    }
    $fuid = $record['fuid'];
  
    // Add roles chosen for user.
    if (isset($form_state['values']['future_user_roles']) && user_access('administer shibboleth future roles')) {
      $assignable_roles = isushibsiteaccess_available_roles();
      foreach ($form_state['values']['future_user_roles'] as $rid => $selected) {
        if ($selected) {
          if ($u) {
            // Existing user. Assign role if user has permission to do so.
            if (user_access('administer users') && user_access('administer permissions')) {
              user_multiple_role_edit(array($u->uid), 'add_role', $rid);
              if ($single_user_being_added) {
                drupal_set_message(t('Added role %role for current user %name.', array('%role' => $assignable_roles[$rid], '%name' => $name)));
              }
            }
            else {
              $cannot_assign[] = $name;
            }
          }
          else {
            // Assign role to future user.
            $record = array(
              'fuid' => $fuid,
              'rid' => $rid,
            );
            drupal_write_record('isushibsiteaccess_roles', $record);
            if ($single_user_being_added) {
              drupal_set_message(t('Added role %role for future user %name.', array('%role' => $assignable_roles[$rid], '%name' => $name)));
            }
          }
        }       
      }
    }  
  }
  if ($count) {
    drupal_set_message(t('%count users were added.', array('%count' => $count)));
  }
  if ($cannot_assign) {
    drupal_set_message(t('Unable to do role assignment for %names because they have already logged into the site. Contact your administrator who can modify role assignments for current users.', array('%names' => implode(', ', $cannot_assign))));
  }
}

/**
 * Form definition for choosing assignable roles from all Drupal roles.
 */
function isushibsiteaccess_form_configuration() {
  // Get list of roles, excluding anonymous user role.
  
  $roles = array_map('check_plain', user_roles(TRUE));
  $checkbox_authenticated = array(
    '#type' => 'checkbox',
    '#title' => $roles[DRUPAL_AUTHENTICATED_RID],
    '#default_value' => TRUE,
    '#disabled' => TRUE,
  );
  unset($roles[DRUPAL_AUTHENTICATED_RID]);
  $assignable_roles = variable_get('isushibsiteaccess_assignable_roles', array());
  $form['account']['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles assignable to future users'),
    '#default_value' => $assignable_roles,
    '#options' => $roles,
    '#access' => $roles && user_access('administer permissions'),
    DRUPAL_AUTHENTICATED_RID => $checkbox_authenticated,
  );

  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function isushibsiteaccess_form_configuration_submit($form, &$form_state) {
  variable_set('isushibsiteaccess_assignable_roles', $form_state['values']['roles']);
  drupal_set_message(t('The changes have been saved.'));  
}

/**
 * Form for editing role assignment for future user.
 */
function isushibsiteaccess_edit_future_user($form, &$form_state, $fuid) {
  if (!is_numeric($fuid)) {
    return;
  }
  
  $available_roles = isushibsiteaccess_available_roles();
  if (!$available_roles) {
    if (user_access('assign permissions')) {
      form_set_error('roles', t('No roles are available to be assigned to this future user. <a href="@url">Make roles available</a>?', array('@url' => url('admin/config/people/isushibsiteaccess/config', array('query' => array('destination' => "admin/config/people/isushibsiteaccess/edit/$fuid"))))));
    }
    else {
      form_set_error('roles', t('No roles are available to be assigned to this future user. Contact your site administrator to make roles available.'));
    }
  }
  
  $name = isushibsiteaccess_get_name($fuid);
      
  $form['roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Roles for future user %name', array('%name' => $name)),
    '#options' => $available_roles,
    '#default_value' => array_keys(isushibsiteaccess_assigned_roles($fuid)),
  );
   
  $form['fuid'] = array(
    '#type' => 'value',
    '#value' => $fuid,
  );   
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $name,
  );
  
  $form['submit'] = array(
   '#type' => 'submit',
   '#value' => t('Submit'),
  );

  return $form;
}

function isushibsiteaccess_edit_future_user_submit($form, &$form_state) {
  $fuid = $form_state['values']['fuid'];
  $name = $form_state['values']['name'];
  db_delete('isushibsiteaccess_roles')
    ->condition('fuid', $fuid)
    ->execute();
    
  foreach ($form_state['values']['roles'] as $rid => $selected) {
    if ($selected) {
     $record = array(
       'fuid' => $fuid,
       'rid' => $rid,
     );
     drupal_write_record('isushibsiteaccess_roles', $record);
    }
  }
  drupal_set_message(t('Role settings for future user %name have been updated.', array('%name' => $name)));
  $form_state['redirect'] = 'admin/config/people/isushibsiteaccess';
}

function isushibsiteaccess_user_delete_confirm($form, &$form_state, $fuid) {
  if (!is_numeric($fuid)) {
    return;
  }

  $name = isushibsiteaccess_get_name($fuid);

  $form['fuid'] = array(
    '#type' => 'value',
    '#value' => $fuid,
  );
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $name,
  );
  return confirm_form(
    $form,
    t('Are you sure you want to delete future user %name?', array('%name' => $name)),
    'admin/config/people/isushibsiteaccess',
    t('This will delete future user %name forever.', array('%name' => $name)),
    t('Delete')
  );
}

function isushibsiteaccess_user_delete_confirm_submit($form, &$form_state) {
  $fuid = $form_state['values']['fuid'];
  $name = $form_state['values']['name'];
  if ($form_state['values']['confirm']) {
    // Delete role records for fuid
    db_delete('isushibsiteaccess_roles')
      ->condition('fuid', $fuid)
      ->execute();
    watchdog('isushibsiteaccess', 'deleted roles for %name', array('%name' => $name));
  
    // Delete record for fuid
    db_delete('isushibsiteaccess_users')
      ->condition('fuid', $fuid)
      ->execute();   
    watchdog('isushibsiteaccess', 'deleted user entry for %name', array('%name' => $name));   
  }
  drupal_set_message(t('Future user %name has been deleted.', array('%name' => $name)));
  $form_state['redirect'] = 'admin/config/people/isushibsiteaccess';
}
