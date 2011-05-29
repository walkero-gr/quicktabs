<?php
// Id:$

/**
 * @file Add Views module hooks to Quicktabs.
 */
 
/**
 * Implementation of hook_views_plugins().
 */
function quicktabs_views_plugins() {
  $path = drupal_get_path('module', 'quicktabs');
  
  return array(
    'style' => array(
      'quicktabs' => array(
        'title' => t('Quicktabs'),
        'help' => t('Display view in Quicktabs.'),
        'handler' => 'quicktabs_style_plugin',
        'path' => "$path/includes",
        'theme' => 'quicktabs_view',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}

function theme_quicktabs_views_render($variables) {
  $tabs = $variables['tabs'];
  $view = $variables['view'];
  foreach ($tabs as $index => &$tab) {
    if (is_array($tab['contents'])) {
      foreach ($tabs[$index]['contents'] as &$content) {
        $content = '<div class="quicktabs-views-group">' . $content . '</div>';
      }
    }
    $tab['contents'] = array('#markup' => $tab['contents']);
  }
  $overrides = array('default_tab' => 0, 'hide_empty_tabs' => 0, 'ajax' => 0, 'style' => $view->style_options['tab_style']);
  $quicktabs = quicktabs_build_quicktabs($variables['name'], $overrides, $tabs);
  return drupal_render($quicktabs);
}
