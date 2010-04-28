<?php
// $Id$

/**
 * Implementation of hook_profile_details().
 */
function nodeone_profile_details() {
  return array(
    'name' => 'NodeOne site',
    'description' => 'Default installtion profile for sites built by NodeOne,',
  );
}

/**
 * Implementation of hook_profile_modules().
 */
function nodeone_profile_modules() {
  return array(
    // Core
    'comment',
    'dblog',
    'help',
    'menu',
    'path',
    'taxonomy',
    'update',

    // Contrib
    'admin',
    'admin_theme',
    'adminrole',
    'better_formats',
    'content',
    'ctm',
    'ctools',
    'features',
    'fieldgroup',
    'filefield',
    'imageapi',
    'imageapi_gd',
    'imagecache',
    'imagecache_ui',
    'imagefield',
    'jquery_ui',
    'jquery_update',
    'number',
    'optionwidgets',
    'pathauto',
    'rules',
    'rules_admin',
    'text',
    'token',
    'semanticviews',
    'strongarm',
    'vertical_tabs',
    'views',
    'views_bulk_operations',
    'views_ui',

    // Developer
    'devel',
    'simpletest',
  );
}


/**
 * Implementation of hook_profile_task_list().
 */
function nodeone_profile_task_list() {
}

/**
 * Implementation of hook_profile_tasks().
 *
 * @todo Find a way to enable page manager pages.
 */
function nodestream_profile_tasks(&$task, $url) {
  // Setup the default Page content type.
  $types = array(
    array(
      'type' => 'page',
      'name' => st('Static page'),
      'module' => 'node',
      'description' => st("Use <em>static pages</em> for your static content, such as an 'About us' page."),
      'custom' => TRUE,
      'modified' => TRUE,
      'locked' => FALSE,
      'help' => '',
      'min_word_count' => '',
      'body_label' => '',
      'has_body' => FALSE,
    ),
  );

  foreach ($types as $type) {
    $type = (object) _node_type_set_defaults($type);
    node_type_save($type);
  }

  // Default page to not be promoted and have comments disabled.
  variable_set('node_options_page', array('status'));
  variable_set('comment_page', COMMENT_NODE_DISABLED);

  // Don't display date and author information for page nodes by default.
  $theme_settings = variable_get('theme_settings', array());
  $theme_settings['toggle_node_info_page'] = FALSE;
  variable_set('theme_settings', $theme_settings);

  // Theme related stuff.
  variable_set('admin_theme', 'garland');
  variable_set('node_admin_theme', 1);
  variable_set('admin_theme_path', "help/*");
  variable_set('admin_theme_admin_theme_batch', 1);

  // Add Plain text input filter
  db_query("INSERT INTO {filter_formats} (name, roles, cache) VALUES ('Plain text', '', 0)");
  $format = db_result(db_query("SELECT MAX(format) FROM {filter_formats}"));
  db_query("INSERT INTO {filters} (format, module, delta, weight) VALUES (%d, 'filter', 0, 0)", $format);

  // Create an admin role.
  db_query("INSERT INTO {role} (rid, name) VALUES (3, 'admin')");

  // Add user 1 to the admin role.
  db_query("INSERT INTO {users_roles} VALUES (1, 3)");

  // Remove all blocks.
  db_query("TRUNCATE {blocks}");

  // Only admin users can create new user accounts on the site.
  variable_set('user_register', 0);

  // No user configurable timezones.
  variable_set('configurable_timezones', 0);

  // First day of week is Monday.
  variable_set('date_first_day', 1);

  // Date formats, the ISO way.
  variable_set('date_format_short', 'Y-m-d H:i');
  variable_set('date_format_short_custom', 'Y-m-d H:i');
  variable_set('date_format_medium', 'D, Y-m-d H:i');
  variable_set('date_format_medium_custom', 'D, Y-m-d H:i');
  variable_set('date_format_long', 'l, j F, Y - H:i');
  variable_set('date_format_long_custom', 'l, j F, Y - H:i');

  // Update the menu router information.
  menu_rebuild();

  // This isn't actually necessary as there are no node_access() entries,
  // but we run it to prevent the "rebuild node access" message from being
  // shown on install.
  node_access_rebuild();

  // Rebuild key tables/caches.
  drupal_flush_all_caches();
}

