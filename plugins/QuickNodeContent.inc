<?php

/**
 * Class for tab content of type "node" - this is for rendering a node as tab
 * content.
 */
class QuickNodeContent extends QuickContent {
  public function options_form($delta) {
    $tab = $this->item;
    $form = array();
    $form['node']['nid'] = array(
      '#type' => 'textfield',
      '#title' => t('Node'),
      '#description' => t('The node ID of the node.'),
      '#maxlength' => 10,
      '#size' => 20,
      '#default_value' => isset($tab['nid']) ? $tab['nid'] : '',
    );
    $form['node']['teaser'] = array(
      '#type' => 'checkbox',
      '#title' => t('Teaser view'),
      '#default_value' => isset($tab['teaser']) ? $tab['teaser'] : 0,
    );
    $form['node']['hide_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide the title of this node'),
      '#default_value' => isset($tab['hide_title']) ? $tab['hide_title'] : 1,
    );
    return $form;
  }
  public function render_item($hide_empty = FALSE) {
    $item = $this->item;
    $output = array();
    if (isset($item['nid'])) {
      $node = node_load($item['nid']);
      if (!empty($node)) {
        if (node_access('view', $node)) {
          $buildmode = $item['teaser'] ? 'teaser' : 'full';
          $nstruct = node_view($node, $buildmode);
          if ($item['hide_title']) {
            $nstruct['#node']->title = NULL;
          }
          $output = $nstruct;
        }
        //elseif (!$hide_empty) {
        //  $output = theme('quicktabs_tab_access_denied', array($item));
        //}
      }
    }
    return $output;
  }
  
  public function ajax_render_item($args) {
    $item = array();
    list($item['nid'], $item['teaser'], $item['hide_title']) = $args;
    $this->item = array_merge($this->item, $item);
    $output = $this->render_item();
    return !empty($output) ? drupal_render($output) : '';
  }
  public function get_ajax_path($name, $index) {
    $path = parent::get_ajax_path($name, $index);
    $path .= '/'. $this->item['nid'];
    return $path;
  }
}