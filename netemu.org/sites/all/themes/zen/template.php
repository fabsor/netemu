<?php
// $Id: template.php,v 1.45 2009/11/05 13:13:45 johnalbin Exp $

/**
 * @file
 * Contains theme override functions and preprocess functions for the Zen theme.
 *
 * IMPORTANT WARNING: DO NOT MODIFY THIS FILE.
 *
 * The base Zen theme is designed to be easily extended by its sub-themes. You
 * shouldn't modify this or any of the CSS or PHP files in the root zen/ folder.
 * See the online documentation for more information:
 *   http://drupal.org/node/193318
 */

// Auto-rebuild the theme registry during theme development.
if (theme_get_setting('zen_rebuild_registry')) {
  drupal_rebuild_theme_registry();
}


/**
 * Implements HOOK_theme().
 */
function zen_theme(&$existing, $type, $theme, $path) {
  // When #341140 is fixed, replace _zen_path() with drupal_get_path().
  include_once './' . _zen_path() . '/zen-internals/template.theme-registry.inc';
  return _zen_theme($existing, $type, $theme, $path);
}

/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 * @return
 *   A string containing the breadcrumb output.
 */
function zen_breadcrumb($breadcrumb) {
  // Determine if we are to display the breadcrumb.
  $show_breadcrumb = theme_get_setting('zen_breadcrumb');
  if ($show_breadcrumb == 'yes' || $show_breadcrumb == 'admin' && arg(0) == 'admin') {

    // Optionally get rid of the homepage link.
    $show_breadcrumb_home = theme_get_setting('zen_breadcrumb_home');
    if (!$show_breadcrumb_home) {
      array_shift($breadcrumb);
    }

    // Return the breadcrumb with separators.
    if (!empty($breadcrumb)) {
      $breadcrumb_separator = theme_get_setting('zen_breadcrumb_separator');
      $trailing_separator = $title = '';
      if (theme_get_setting('zen_breadcrumb_title')) {
        if ($title = drupal_get_title()) {
          $trailing_separator = $breadcrumb_separator;
        }
      }
      elseif (theme_get_setting('zen_breadcrumb_trailing')) {
        $trailing_separator = $breadcrumb_separator;
      }
      return '<div class="breadcrumb">' . implode($breadcrumb_separator, $breadcrumb) . "$trailing_separator$title</div>";
    }
  }
  // Otherwise, return an empty string.
  return '';
}

/**
 * Implements theme_menu_item_link()
 */
function zen_menu_item_link($link) {
  if (empty($link['localized_options'])) {
    $link['localized_options'] = array();
  }

  // If an item is a LOCAL TASK, render it as a tab
  if ($link['type'] & MENU_IS_LOCAL_TASK) {
    $link['title'] = '<span class="tab">' . check_plain($link['title']) . '</span>';
    $link['localized_options']['html'] = TRUE;
  }

  return l($link['title'], $link['href'], $link['localized_options']);
}

/**
 * Duplicate of theme_menu_local_tasks() but adds clearfix to tabs.
 */
function zen_menu_local_tasks() {
  $output = '';

  if ($primary = menu_primary_local_tasks()) {
    $output .= '<ul class="tabs primary clearfix">' . $primary . '</ul>';
  }
  if ($secondary = menu_secondary_local_tasks()) {
    $output .= '<ul class="tabs secondary clearfix">' . $secondary . '</ul>';
  }

  return $output;
}

/**
 * Return a set of blocks available for the current user.
 *
 * @param $region
 *   Which set of blocks to retrieve.
 * @param $show_blocks
 *   A boolean to determine whether to render sidebar regions or not. Should be
 *   the same value as passed to theme_page.
 * @return
 *   A string containing the themed blocks for this region.
 *
 * @see zen_show_blocks_discovery()
 */
function zen_blocks($region, $show_blocks = NULL) {
  // Since Drupal 6 doesn't pass $show_blocks to theme_blocks, we manually call
  // theme('blocks', NULL, $show_blocks) so that this function can remember the
  // value on later calls.
  static $render_sidebars = TRUE;
  if (!is_null($show_blocks)) {
    $render_sidebars = $show_blocks;
  }

  // If zen_blocks was called with a NULL region, its likely we were just
  // setting the $render_sidebars static variable.
  if ($region) {
    $output = '';

    // If $renders_sidebars is FALSE, don't render any region whose name begins
    // with "sidebar_".
    if (($render_sidebars || (strpos($region, 'sidebar_') !== 0)) && ($list = block_list($region))) {
      foreach ($list as $key => $block) {
        // $key == module_delta
        $output .= theme('block', $block);
      }
    }

    // Add any content assigned to this region through drupal_set_content() calls.
    $output .= drupal_get_content($region);

    $elements['#children'] = $output;
    $elements['#region'] = $region;

    return $output ? theme('region', $elements) : '';
  }
}

/**
 * Examine the $show_blocks variable before template_preprocess_page() is called.
 *
 * @param $vars
 *   An array of variables to pass to the page template.
 *
 * @see zen_blocks()
 */
function zen_show_blocks_discovery(&$vars) {
  if ($vars['show_blocks'] == FALSE) {
    // Allow zen_blocks() to statically cache the $show_blocks variable. A TRUE
    // value is assumed, so we only need to override when $show_blocks is FALSE.
    theme('blocks', NULL, FALSE);
  }
}

/**
 * Override or insert variables into templates before other preprocess functions have run.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered.
 */
function zen_preprocess(&$vars, $hook) {
  // In D6, the page.tpl uses a different variable name to hold the classes.
  $key = ($hook == 'page' || $hook == 'maintenance_page') ? 'body_classes' : 'classes';

  // Create a D7-standard classes_array variable.
  if (array_key_exists($key, $vars)) {
    // Views (and possibly other modules) have templates with a $classes
    // variable that isn't a string, so we leave those variables alone.
    if (is_string($vars[$key])) {
      $vars['classes_array'] = explode(' ', $vars[$key]);
      unset($vars[$key]);
    }
  }
  else {
    $vars['classes_array'] = array($hook);
  }
}

/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
function zen_preprocess_page(&$vars, $hook) {
  // If the user is silly and enables Zen as the theme, add some styles.
  if ($GLOBALS['theme'] == 'zen') {
    include_once './' . _zen_path() . '/zen-internals/template.zen.inc';
    _zen_preprocess_page($vars, $hook);
  }
  // Add conditional stylesheets.
  elseif (!module_exists('conditional_styles')) {
    $vars['styles'] .= $vars['conditional_styles'] = variable_get('conditional_styles_' . $GLOBALS['theme'], '');
  }

  // Classes for body element. Allows advanced theming based on context
  // (home page, node of certain type, etc.)
  // Remove the mostly useless page-ARG0 class.
  if ($index = array_search(preg_replace('![^abcdefghijklmnopqrstuvwxyz0-9-_]+!s', '', 'page-'. drupal_strtolower(arg(0))), $vars['classes_array'])) {
    unset($vars['classes_array'][$index]);
  }
  if (!$vars['is_front']) {
    // Add unique class for each page.
    $path = drupal_get_path_alias($_GET['q']);
    $vars['classes_array'][] = drupal_html_class('page-' . $path);
    // Add unique class for each website section.
    list($section, ) = explode('/', $path, 2);
    if (arg(0) == 'node') {
      if (arg(1) == 'add') {
        $section = 'node-add';
      }
      elseif (is_numeric(arg(1)) && (arg(2) == 'edit' || arg(2) == 'delete')) {
        $section = 'node-' . arg(2);
      }
    }
    $vars['classes_array'][] = drupal_html_class('section-' . $section);
  }
  if (theme_get_setting('zen_wireframes')) {
    $vars['classes_array'][] = 'with-wireframes'; // Optionally add the wireframes style.
  }
  // We need to re-do the $layout and body classes because
  // template_preprocess_page() assumes sidebars are named 'left' and 'right'.
  $vars['layout'] = 'none';
  if (!empty($vars['sidebar_first'])) {
    $vars['layout'] = 'first';
  }
  if (!empty($vars['sidebar_second'])) {
    $vars['layout'] = ($vars['layout'] == 'first') ? 'both' : 'second';
  }
  // If the layout is 'none', then template_preprocess_page() will already have
  // set a 'no-sidebars' class since it won't find a 'left' or 'right' sidebar.
  if ($vars['layout'] != 'none') {
    // Remove the incorrect 'no-sidebars' class.
    if ($index = array_search('no-sidebars', $vars['classes_array'])) {
      unset($vars['classes_array'][$index]);
    }
    // Set the proper layout body classes.
    if ($vars['layout'] == 'both') {
      $vars['classes_array'][] = 'two-sidebars';
    }
    else {
      $vars['classes_array'][] = 'one-sidebar';
      $vars['classes_array'][] = 'sidebar-' . $vars['layout'];
    }
  }
}

/**
 * Override or insert variables into the maintenance page template.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("maintenance_page" in this case.)
 */
function zen_preprocess_maintenance_page(&$vars, $hook) {
  // If Zen is the maintenance theme, add some styles.
  if ($GLOBALS['theme'] == 'zen') {
    include_once './' . _zen_path() . '/zen-internals/template.zen.inc';
    _zen_preprocess_page($vars, $hook);
  }
  // Add conditional stylesheets.
  elseif (!module_exists('conditional_styles')) {
    $vars['styles'] .= $vars['conditional_styles'] = variable_get('conditional_styles_' . $GLOBALS['theme'], '');
  }

  // Classes for body element. Allows advanced theming based on context
  // (home page, node of certain type, etc.)
  $vars['body_classes_array'] = explode(' ', $vars['body_classes']);
}

/**
 * Override or insert variables into the node templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
function zen_preprocess_node(&$vars, $hook) {
  // Create the build_mode variable.
  switch ($vars['node']->build_mode) {
    case NODE_BUILD_NORMAL:
      $vars['build_mode'] = $vars['teaser'] ? 'teaser' : 'full';
      break;
    case NODE_BUILD_PREVIEW:
      $vars['build_mode'] = 'preview';
      break;
    case NODE_BUILD_SEARCH_INDEX:
      $vars['build_mode'] = 'search_index';
      break;
    case NODE_BUILD_SEARCH_RESULT:
      $vars['build_mode'] = 'search_result';
      break;
    case NODE_BUILD_RSS:
      $vars['build_mode'] = 'rss';
      break;
    case NODE_BUILD_PRINT:
      $vars['build_mode'] = 'print';
      break;
  }

  // Create the user_picture variable.
  $vars['user_picture'] = $vars['picture'];

  // Create the Drupal 7 $display_submitted variable.
  $vars['display_submitted'] = theme_get_setting('toggle_node_info_' . $vars['node']->type);

  // Special classes for nodes.
  // Class for node type: "node-type-page", "node-type-story", "node-type-my-custom-type", etc.
  $vars['classes_array'][] = drupal_html_class('node-type-' . $vars['type']);
  if ($vars['promote']) {
    $vars['classes_array'][] = 'node-promoted';
  }
  if ($vars['sticky']) {
    $vars['classes_array'][] = 'node-sticky';
  }
  if (!$vars['status']) {
    $vars['classes_array'][] = 'node-unpublished';
    $vars['unpublished'] = TRUE;
  }
  else {
    $vars['unpublished'] = FALSE;
  }
  if ($vars['uid'] && $vars['uid'] == $GLOBALS['user']->uid) {
    $vars['classes_array'][] = 'node-by-viewer'; // Node is authored by current user.
  }
  if ($vars['teaser']) {
    $vars['classes_array'][] = 'node-teaser'; // Node is displayed as teaser.
  }
  if (isset($vars['preview'])) {
    $vars['classes_array'][] = 'node-preview';
  }
}

/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
function zen_preprocess_comment(&$vars, $hook) {
  include_once './' . _zen_path() . '/zen-internals/template.comment.inc';
  _zen_preprocess_comment($vars, $hook);
}

/**
 * Preprocess variables for region.tpl.php
 *
 * Prepare the values passed to the theme_region function to be passed into a
 * pluggable template engine. Uses the region name to generate a template file
 * suggestions. If none are found, the default region.tpl.php is used.
 *
 * @see region.tpl.php
 */
function zen_preprocess_region(&$vars, $hook) {
  // Create the $content variable that templates expect.
  $vars['content'] = $vars['elements']['#children'];
  $vars['region'] = $vars['elements']['#region'];

  // Setup the default classes.
  $region = 'region-' . str_replace('_', '-', $vars['region']);
  $vars['classes_array'] = array('region', $region);

  // Sidebar regions get a common template suggestion a couple extra classes.
  if (strpos($vars['region'], 'sidebar_') === 0) {
    $vars['template_files'][] = 'region-sidebar';
    $vars['classes_array'][] = 'column';
    $vars['classes_array'][] = 'sidebar';
  }

  // Setup the default template suggestion.
  $vars['template_files'][] = $region;
}

/**
 * Override or insert variables into the block templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
function zen_preprocess_block(&$vars, $hook) {
  $block = $vars['block'];

  // Drupal 7 uses a $content variable instead of $block->content.
  $vars['content'] = $block->content;
  // Drupal 7 should use a $title variable instead of $block->subject.
  $vars['title'] = $block->subject;

  // Special classes for blocks.
  $vars['classes_array'][] = 'block-' . $block->module;
  $vars['classes_array'][] = 'region-' . $vars['block_zebra'];
  $vars['classes_array'][] = $vars['zebra'];
  $vars['classes_array'][] = 'region-count-' . $vars['block_id'];
  $vars['classes_array'][] = 'count-' . $vars['id'];

  $vars['edit_links_array'] = array();
  if (theme_get_setting('zen_block_editing') && user_access('administer blocks')) {
    include_once './' . _zen_path() . '/zen-internals/template.block-editing.inc';
    zen_preprocess_block_editing($vars, $hook);
    $vars['classes_array'][] = 'with-block-editing';
  }
}

/**
 * Override or insert variables into templates after preprocess functions have run.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered.
 */
function zen_process(&$vars, $hook) {
  if (array_key_exists('classes_array', $vars)) {
    $vars['classes'] = implode(' ', $vars['classes_array']);
  }
}

/**
 * Override or insert variables into the block templates after preprocess functions have run.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
function zen_process_block(&$vars, $hook) {
  $vars['edit_links'] = !empty($vars['edit_links_array']) ? '<div class="edit">' . implode(' ', $vars['edit_links_array']) . '</div>' : '';
}

if (!function_exists('drupal_html_class')) {
  /**
   * Prepare a string for use as a valid class name.
   *
   * Do not pass one string containing multiple classes as they will be
   * incorrectly concatenated with dashes, i.e. "one two" will become "one-two".
   *
   * @param $class
   *   The class name to clean.
   * @return
   *   The cleaned class name.
   */
  function drupal_html_class($class) {
    // By default, we filter using Drupal's coding standards.
    $class = strtr(drupal_strtolower($class), array(' ' => '-', '_' => '-', '/' => '-', '[' => '-', ']' => ''));

    // http://www.w3.org/TR/CSS21/syndata.html#characters shows the syntax for valid
    // CSS identifiers (including element names, classes, and IDs in selectors.)
    //
    // Valid characters in a CSS identifier are:
    // - the hyphen (U+002D)
    // - a-z (U+0030 - U+0039)
    // - A-Z (U+0041 - U+005A)
    // - the underscore (U+005F)
    // - 0-9 (U+0061 - U+007A)
    // - ISO 10646 characters U+00A1 and higher
    // We strip out any character not in the above list.
    $class = preg_replace('/[^\x{002D}\x{0030}-\x{0039}\x{0041}-\x{005A}\x{005F}\x{0061}-\x{007A}\x{00A1}-\x{FFFF}]/u', '', $class);

    return $class;
  }
} /* End of drupal_html_class conditional definition. */

if (!function_exists('drupal_html_id')) {
  /**
   * Prepare a string for use as a valid HTML ID and guarantee uniqueness.
   *
   * @param $id
   *   The ID to clean.
   * @return
   *   The cleaned ID.
   */
  function drupal_html_id($id) {
    $id = strtr(drupal_strtolower($id), array(' ' => '-', '_' => '-', '[' => '-', ']' => ''));

    // As defined in http://www.w3.org/TR/html4/types.html#type-name, HTML IDs can
    // only contain letters, digits ([0-9]), hyphens ("-"), underscores ("_"),
    // colons (":"), and periods ("."). We strip out any character not in that
    // list. Note that the CSS spec doesn't allow colons or periods in identifiers
    // (http://www.w3.org/TR/CSS21/syndata.html#characters), so we strip those two
    // characters as well.
    $id = preg_replace('/[^A-Za-z0-9\-_]/', '', $id);

    return $id;
  }
} /* End of drupal_html_id conditional definition. */

/**
 * Returns the path to the Zen theme.
 *
 * drupal_get_filename() is broken; see #341140. When that is fixed in Drupal 6,
 * replace _zen_path() with drupal_get_path('theme', 'zen').
 */
function _zen_path() {
  static $path = FALSE;
  if (!$path) {
    $matches = drupal_system_listing('zen\.info$', 'themes', 'name', 0);
    if (!empty($matches['zen']->filename)) {
      $path = dirname($matches['zen']->filename);
    }
  }
  return $path;
}
