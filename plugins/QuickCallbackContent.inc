<?php

/**
 * Class for tab content of type "callback" - this is for rendering the contents
 * of some menu callback function as tab content.
 */
class QuickCallbackContent extends QuickContent {
  
  public static function getType() {
    return 'callback';
  }
  
  public function __construct($item) {
    parent::__construct($item);

    if (isset($item['path'])) {
      $this->settings['ajax_path'] = urlencode($item['path']);
    }
  }

  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();
    $form['callback']['path'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tab['path']) ? $tab['path'] : '',
      '#title' => t('Path'),
    );
    return $form;
  }

  public function render($hide_empty = FALSE, $args = array()) {
    if ($this->rendered_content) {
      return $this->rendered_content;
    }
    $item = $this->settings;
    if (!empty($args)) {
      // The args have been passed in from an ajax request.
      $item['path'] = urldecode($args[0]);
      $_GET['q'] = $item['path'];
    }
    
    $output = array();
    if (isset($item['path'])) {
      $response = menu_execute_active_handler($item['path'], FALSE);
      if (!is_array($response)) {
        if (is_int($response)) {
          if (MENU_ACCESS_DENIED == $response && !$hide_empty) {
            $output['#markup'] = theme('quicktabs_tab_access_denied', array('tab' => $item));
          }
          // For any other integer response form the menu callback, we'll just
          // return an empty array.
        }
        else {
          $output = array('#markup' => $output);
        }
      }
      else {
        $output = $response;
      }
    }
    $this->rendered_content = $output;
    return $output;
  }

  public function getAjaxKeys() {
    return array('ajax_path');
  }
}