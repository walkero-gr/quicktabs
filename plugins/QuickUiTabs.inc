<?php

/**
 * Renders the content using the jQuery UI Tabs widget.
 */
class QuickUiTabs extends QuickSet {
  
  public static $type = 'ui_tabs';
  
  public function render() {
    
    // This display style doesn't currently support the ajax option
    $this->options['ajax'] = 0;

    try {
      $this->prepare_contents();
    }
    catch (InvalidQuickSetException $e) {
      return array('#markup' => $e->getMessage());
    }

    $active_tab = $this->get_active_tab();
    if ($tabs = $this->build_tablinks($active_tab)) {
      $qt_name = $this->name;
      // Create the quicktabs renderable array and add the necessary css and js to it
      $qt_ra = array(
        '#attached' => array(
          'css' => array(
            //array('data' => drupal_get_path('module', 'quicktabs') .'/css/quicktabs.css'),
          ),
          'js' => array(
            array(
              'data' => "jQuery(function() { jQuery('.quicktabs-wrapper').tabs({selected: $active_tab})})",
              'type' => 'inline',
            )
          ),
          'library' => array(
            array('system', 'ui.tabs'),
          ),
        ),
        'content' => array(
          '#theme' => 'qt_ui_tabs',
          '#options' => array('attributes' => array(
            'id' => 'quicktabs-' . $qt_name,
            'class' => 'quicktabs-wrapper',
          )),
          'tabs' => array('#theme' => 'qt_ui_tabs_tabset', '#options' => array('active' => $active_tab), 'tablinks' => $tabs),
        ),
      );

      // Render all tab content.
      $qt_ra['content']['divs'] = array();
      foreach ($this->contents as $key => $tab) {
        $attribs = array(
          'id' => 'qt-'. $qt_name .'-ui-tabs' . ($key+1),
        );
        $qt_ra['content']['divs'][] = array(
          '#prefix' => '<div '. drupal_attributes($attribs) .'>',
          '#suffix' => '</div>',
          'content' => $tab_content = isset($tab['rendered']) ? $tab['rendered'] : $tab['builder']->render_item(),
        );
      }
      return $qt_ra;
    }
    return array();
  }

  
  /**
   * Build the actual tab links, with appropriate href, title and attributes.
   * 
   * @param $active_tab The index of the active tab.
   */
  protected function build_tablinks($active_tab) {
    $tabs = array();
    $tabs_count = count($this->contents);
    if ($tabs_count <= 0) {
      return NULL;
    }
    $qt_name = $this->name;
    foreach ($this->contents as $i => $tab) {
      // If we use l() here or a render array of type 'link', the '#' symbol will
      // be escaped. Sad panda is sad.
      $href = '#qt-'. $qt_name .'-ui-tabs' . ($i+1);
      $tablink = array(
	'#markup' => '<a href="'. $href .'">'. $tab['title'] .'</a>',
      );
      $tabs[] = $tablink;
    }
    return $tabs;
  }
}
