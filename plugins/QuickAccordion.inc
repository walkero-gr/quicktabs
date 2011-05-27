<?php

/**
 * Renders the content using the jQuery UI Accordion widget.
 */
class QuickAccordion extends QuickSet {
  
  public static $type = 'accordion';
  
  public function render() {
    
    // This display style doesn't currently support the ajax option
    $this->options['ajax'] = 0;
    
    $active_tab = $this->get_active_tab();
    
    $qt_name = $this->name;
    // Create the quicktabs renderable array and add the necessary css and js to it
    $qt_ra = array(
      '#attached' => array(
        'library' => array(
          array('system', 'ui.accordion'),
        ),
        'js' => array(
          array('data' => drupal_get_path('module', 'quicktabs') . '/js/quicktabs.js'),
        ),
      ),
      'content' => array(
        '#theme' => 'qt_accordion',
        '#options' => array('attributes' => array(
          'id' => 'quickset-' . $qt_name,
          'class' => array('quick-accordion','quickset-wrapper'),
        )),
      ),
    );
    $javascript = drupal_add_js();
    if (!isset($javascript['setting'][1]['quicktabs']) || !array_key_exists('qt_' . $qt_name, $javascript['setting'][1]['quicktabs'])) {
      $quicktabs_array = array('name' => $qt_name, 'active_tab' => $active_tab);
      $qt_ra['#attached']['js'][] = array('data' => array('quicktabs' => array('qt_' . $qt_name => $quicktabs_array)), 'type' => 'setting');
    }
    // Render all tab content.
    $qt_ra['content']['divs'] = array();
    foreach ($this->contents as $key => $item) {
      $qt_ra['content']['divs'][] = array(
        '#prefix' => '<h3><a href= "#">'. $item['title'] .'</a></h3><div>',
        '#suffix' => '</div>',
        'content' => isset($item['rendered']) ? $item['rendered'] : $item['builder']->render_item(),
      );
    }
    return $qt_ra;
  }

}