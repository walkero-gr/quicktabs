<?php

/**
 * Class for tab content of type "qtabs" - this is for rendering a QuickSet instance
 * as the tab content of another QuickSet instance.
 */
class QuickQtabsContent extends QuickContent {
  public function options_form($delta) {
    $tab = $this->item;
    $form = array();
    $form['qtabs']['machine_name'] = array(
      '#type' => 'textfield',
      '#title' => t('Quicktabs instance'),
      '#description' => t('The machine name of the existing Quicktabs instance.'),
      '#maxlength' => 10,
      '#size' => 20,
      '#default_value' => isset($tab['machine_name']) ? $tab['machine_name'] : '',
    );
    return $form;
  }
  public function render($hide_empty = FALSE, $args = array()) {
    if ($this->rendered_content) return $this->rendered_content;
    $item = $this->item;
    if (!empty($args)) {
      // The args have been passed in from an ajax request.
      $item['machine_name'] = $args[0];
    }

    $output = array();
    if (isset($item['machine_name'])) {
      if ($quicktabs = quicktabs_load($item['machine_name'])) {
        $contents = $quicktabs->tabs;
        $name = $quicktabs->machine_name;
        unset($quicktabs->tabs, $quicktabs->machine_name);
        $options = (array) $quicktabs;
        if ($qt = QuickSet::rendered_quickset_factory($name, $contents, $quicktabs->renderer, $options)) {
          $output = $qt->render();
        }
      }
    }
    return $output;
  }
  
  //public function ajax_render_item($args) {
  //  $item = array();
  //  $item['machine_name'] = $args[0];
  //  $this->item = array_merge($this->item, $item);
  //  $output = $this->render_item();
  //  return !empty($output) ? drupal_render($output) : '';
  //}
  
  //public function get_ajax_path($name, $index) {
  //  $path = parent::get_ajax_path($name, $index);
  //  return $path . '/'. $this->item['machine_name'];
  //}

  public function getAjaxArgs() {
    return array($this->item['machine_name']);
  }
}