<?php

/**
 * Renders the content using the original Quicktabs mechanism of previous versions.
 * Includes support for ajax rendered content.
 */
class QuickQuicktabs extends QuickSet {
  
  public function render() {

    // All we really want to do here is copy $this->contents into a tab_settings array
    // to pass into our js, but without the "contents" element because that would cause
    // mad recursion; however, due to this php weirdness http://bugs.php.net/bug.php?id=20993,
    // assignment would create a reference to $this->contents, which is not what we want.
    $tab_settings = array();
    foreach ($this->contents as $content) {
      $item = array();
      foreach ($content as $name => $element) {
        // Certain properties should not be included in the js settings array.
        if (!in_array($name, array('contents', 'rendered', 'builder'))) {
          $item[$name] = $element;
        }
      }
      $tab_settings[] = $item;
    }

    $active_tab = $this->get_active_tab();
    if ($tabs = $this->build_tablinks($active_tab)) {
      $qt_name = $this->name;
      $style = isset($this->options['style']) ? $this->options['style'] : 'nostyle';
      if ($style == 'default') $style = variable_get('quicktabs_tabstyle', 'nostyle');
      $qt_ra = array(
        '#attached' => array(
          'css' => array(
            array('data' => drupal_get_path('module', 'quicktabs') .'/css/quicktabs.css'),
            array('data' => quicktabs_get_css($this->options['style'])),
          ),
          'js' => array(
            array('data' => drupal_get_path('module', 'quicktabs') . '/js/quicktabs.js'),
            array('data' => 'misc/progress.js', 'weight' => JS_LIBRARY),
          ),
        ),
        'content' => array(
          '#theme' => 'qt_quicktabs',
          '#options' => array('attributes' => array(
            'id' => 'quicktabs-' . $qt_name,
            'class' => 'quicktabs-wrapper quicktabs-style-' . drupal_strtolower($style),
          )),
          'tabs' => array('#theme' => 'qt_quicktabs_tabset', '#options' => array('active' => $active_tab, 'style' => drupal_strtolower($style)), 'tablinks' => $tabs),
          // The main content area, each quicktab container needs a unique id.
          'container' => array(
            '#prefix' => '<div id="quicktabs-container-' . $qt_name .'" class="quicktabs_main quicktabs-style-' . drupal_strtolower($style) .'">',
            '#suffix' => '</div>',
          ),
        ),
      );
      // Add our JS settings
      $javascript = drupal_add_js();
      foreach ($javascript['settings']['data'] as $key => $settings) {
        if (key($settings) == 'quicktabs') {
          $qtkey = $key;
          break;
        }
      }
      if (!isset($qtkey) || !array_key_exists('qt_' . $qt_name, $javascript['settings']['data'][$qtkey]['quicktabs'])) {
        $quicktabs_array = array_merge(array('name' => $qt_name, 'tabs' => $tab_settings), $this->options);
        $qt_ra['#attached']['js'][] = array('data' => array('quicktabs' => array('qt_' . $qt_name => $quicktabs_array)), 'type' => 'setting');
      }
    
      if ($this->options['ajax']) {
        // Render only the active tab content.
        $tab = $this->contents[$active_tab];
        $attribs = array(
          'id' => 'quicktabs-tabpage-'. $qt_name . '-'. $active_tab,
          'class' => array('quicktabs-tabpage'),
        );
        $qt_ra['content']['container']['active'] = array(
          '#prefix' => '<div '. drupal_attributes($attribs) .'>',
          '#suffix' => '</div>',
          'content' =>  isset($tab['rendered']) ? $tab['rendered'] : $tab['builder']->render_item(),
        );
      }
      else {
        // Render all tab contents.
        $qt_ra['content']['container']['divs'] = array();
        foreach ($this->contents as $key => $tab) {
          $attribs = array(
            'id' => 'quicktabs-tabpage-'. $qt_name . '-'. $key,
            'class' => array('quicktabs-tabpage', ($active_tab == $key ? '' : 'quicktabs-hide')),
          );
          $qt_ra['content']['container']['divs'][] = array(
            '#prefix' => '<div '. drupal_attributes($attribs) .'>',
            '#suffix' => '</div>',
            'content' => isset($tab['rendered']) ? $tab['rendered'] : $tab['builder']->render_item(),
          );
        }
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
  
    foreach ($this->contents as $i => $tab) {
      $tablink = array(
        '#type' => 'link',
        '#title' => $tab['title'],
        '#href' =>  $_GET['q'],
        '#options' => $this->construct_link_options($i),
      );
      if ($this->options['ajax']) {
        $type = $tab['type'];
        $ajax_href = $tab['builder']->get_ajax_path($this->name, $i);
        $tablink['#ajax'] = array(
          'progress' => array('message' => '', 'type' => 'throbber'),
          'path' => $ajax_href,
        );
      }
      $tabs[] = $tablink;
    }
  
    return $tabs;
  }
  
  /**
   * Helper function to construct link options for tab links.
   */
  protected function construct_link_options($tabkey) {
    $tab = $this->contents[$tabkey];
    $qt_name = $this->name;
    $ajax = $this->options['ajax'];
    $id = 'quicktabs-tab-' . implode('-', array($qt_name, $tabkey));
  
    // Need to construct the correct querystring for the tab links.
    $query = drupal_get_query_parameters(NULL, array("qt-$qt_name", 'q', 'page'));
    $query["qt-{$qt_name}"] = $tabkey;
  
    $class = array();
    if ($ajax) {
      $class[] = 'qt-ajax-tab';
    }
    else {
      $class[] = 'qt-tab';
    }
  
    $link_options = array(
      'attributes' => array(
        'id' => $id,
        'class' => $class,
      ),
      'query' => $query,
      'fragment' => 'qt-' . $qt_name,
    );
    return $link_options;
  }
}