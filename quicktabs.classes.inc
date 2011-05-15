<?php

class QuickieFactory {
  
  function getQuickie($name, $contents, $renderer, $options) {
    $renderer = ucfirst($renderer);
    $func = create_function('$c', 'return strtoupper($c[1]);');
    $type = preg_replace_callback('/_([a-z])/', $func, $renderer);
    
    $class_name = "Quick$type";
    if (class_exists($class_name)) {
      return new $class_name($name, $contents, $options);
    }
  }
}

abstract class Quickie {
  
  protected $name;
  protected $contents;
  protected $options;

  
  function __construct($name, $contents, $options) {
    $this->name = $name;
    $this->contents = $contents;
    $this->options = $options;
  }
  
  abstract function render();
  
  protected function prepare_contents() {

    if ($this->options['hide_empty_tabs'] && !$this->options['ajax']) {
      // Check if any tabs need to be hidden because of empty content.
      foreach ($this->contents as $key => $tab) {
        $contents = isset($tab['contents']) ? $tab['contents'] : self::render_content_item($tab, TRUE);
        if (empty($contents)) {
          unset($this->contents[$key]);
        }
        else {
          $this->contents[$key]['rendered'] = $contents;
        }
      }
      $this->contents = array_merge($this->contents);
    }
    elseif ($this->options['ajax']) {
      // Make sure that there is at most 1 "custom" tab and it is the default tab.
      // Custom content cannot be rendered via ajax.
      $custom = FALSE; // keep track of whether we have found a custom tab.
      foreach ($this->contents as $key => &$tab) {
        if ($tab['type'] == 'custom') {
          if (!$custom) {
            $custom = TRUE;
            $this->options['default_tab'] = $key;
            $tab['rendered'] = $tab['contents'];
          }
          else {
            throw new InvalidQuickieException('The contents specified cannot be rendered via ajax.');
          }
        }
      }
    }
    else {
      // Copy "contents" into the "rendered" property for custom tabs.
      foreach ($this->contents as $key => &$tab) {
        if ($tab['type'] == 'custom') {
          $tab['rendered'] = $tab['contents'];
        }
      }
    }
    $this->prepare_views_args();
  }
  
  /**
  * Convert %-style View arguments from the URL into an array
  */
  protected function prepare_views_args() {
    foreach ($this->contents as $key => &$content) {
      if ($content['type'] == 'view') {
        if ($this->options['ajax']) views_add_js('ajax_view');
        
        $url_args = arg();
        $args = $content['args'];
  
        foreach ($url_args as $id => $arg) {
          $args = str_replace("%$id", $arg, $args);
        }
        $args = preg_replace(',/?(%\d),', '', $args);
        $args = $args ? explode('/', $args) : array();
  
        $content['args'] = $args;
      }
    }
  }
  
  protected function get_active_tab() {
    $active_tab = isset($this->options['default_tab']) ? $this->options['default_tab'] : key($this->contents);
    $active_tab = isset($_GET['qt-' . $this->name]) ? $_GET['qt-' . $this->name] : $active_tab;
    $active_tab = (isset($active_tab) && isset($this->contents[$active_tab])) ? $active_tab : 0;
    return $active_tab;
  }
  
  /**
  * Render individual tab content.
  */
  static function render_content_item($item, $hide_empty = FALSE) {
    static $cache;
    $cachekey = md5(serialize($item));
    if (isset($cache[$cachekey])) {
      return $cache[$cachekey];
    }
    $output = array();
    if (isset($item['type'])) {
      $func = 'build_content_'. $item['type'];
      if (is_callable(array('self', $func))) {
        $output = self::$func($item, $hide_empty);
        $cache[$cachekey] = $output;
      }
    }
    return $output;
  }
  
  static function build_content_qtabs($item, $hide_empty = FALSE) {
    $output = '';
    if (isset($item['machine_name'])) {
      if ($quicktabs = quicktabs_load($item['machine_name'])) {
        $contents = $quicktabs->tabs;
        $name = $quicktabs->machine_name;
        unset($quicktabs->tabs, $quicktabs->machine_name);
        $options = (array) $quicktabs;
        $qf = new QuickieFactory();
        $qt = $qf->getQuickie($name, $contents, 'tabs', $options);
        $output = $qt->render();
      }
    }
    return $output;
  }
  
  static function build_content_view($item, $hide_empty = FALSE) {
    $output = array();
    if (isset($item['vid'])) {
      if (module_exists('views')) {
        if ($view = views_get_view($item['vid'])) {
          if ($view->access($item['display'])) {
            $view->set_display($item['display']);
            $view->set_arguments($item['args']);
            $view_output = $view->preview();
            if (!empty($view->result) || $view->display_handler->get_option('empty') || !empty($view->style_plugin->definition['even empty'])) {
              $output['#markup'] = $view_output;
            }
          }
          elseif (!$hide_empty) {
            $output = theme('quicktabs_tab_access_denied', array('tab' => $tab));
          }
          $view->destroy();
        }
      }
      elseif (!$hide_empty) {
        $output['#markup'] = t('Views module is not enabled, cannot display content.');
      }
    }
    return $output;
  }
  
  static function build_content_block($item, $hide_empty = FALSE) {
    $output = array();
    if (isset($item['bid'])) {
      if (module_exists('block')) {
        $pos = strpos($item['bid'], '_delta_');
        $module = drupal_substr($item['bid'], 0, $pos);
        $delta = drupal_substr($item['bid'], $pos + 7);
  
        $block = block_load($module, $delta);
        $block->region = 'quicktabs_tabpage';
  
        if ($block_arr = _block_render_blocks(array($block))) {
          if ($item['hide_title']) {
            $block_arr["{$block->module}_{$block->delta}"]->subject = FALSE;
          }
          if (!empty($block_arr["{$block->module}_{$block->delta}"]->content)) {
            $build = _block_get_renderable_array($block_arr);
            $output = $build;
          }
        }
      }
      elseif (!$hide_empty) {
        $output['#markup'] = t('Block module is not enabled, cannot display content.');
      }
    }
    return $output;
  }
  
  static function build_content_node($item, $hide_empty = FALSE) {
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
}

class QuickUiTabs extends Quickie {
  
  static $type = 'ui_tabs';
  
  function render() {
    
    // This display style doesn't currently support the ajax option
    $this->options['ajax'] = 0;

    try {
      // Ensures empty tabs are hidden, views args are converted from the url,
      // and custom content won't be attempted to be ajax-rendered.
      $this->prepare_contents();
    }
    catch (InvalidQuickieException $e) {
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
          'content' => $tab_content = isset($tab['rendered']) ? $tab['rendered'] : self::render_content_item($tab),
        );
      }
      return $qt_ra;
    }
    return array();
  }

  
  /**
   * @todo: Enter description here ...
   * @param $active_tab
   */
  protected function build_tablinks($active_tab) {
    $tabs = array();
    $tabs_count = count($this->contents);
    if ($tabs_count <= 0) {
      return NULL;
    }
    $qt_name = $this->name;
    foreach ($this->contents as $i => $tab) {
      $href = '#qt-'. $qt_name .'-ui-tabs' . ($i+1);
      $tablink = array(
	'#markup' => '<a href="'. $href .'">'. $tab['title'] .'</a>',
      );
      $tabs[] = $tablink;
    }
    return $tabs;
  }
}

class QuickQuicktabs extends Quickie {
  
  static $type = 'quicktabs';
  
  function render() {

    try {
      // Ensures empty tabs are hidden, views args are converted from the url,
      // and custom content won't be attempted to be ajax-rendered.
      $this->prepare_contents();
    }
    catch (InvalidQuickieException $e) {
      return array('#markup' => $e->getMessage());
    }

    // All we really want to do here is copy $this->contents into a tab_settings array
    // to pass into our js, but unsetting the contents elements because that would cause
    // crazy recursion; however, due to this crazy php weirdness http://bugs.php.net/bug.php?id=20993,
    // assignment would create a reference to $this->contents, which is not what we want.
    $tab_settings = array();
    foreach ($this->contents as $content) {
      $item = array();
      foreach ($content as $name => $element) {
        if (!in_array($name, array('contents', 'rendered'))) {
          $item[$name] = $element;
        }
      }
      $tab_settings[] = $item;
    }

    $active_tab = $this->get_active_tab();
    if ($tabs = $this->build_tablinks($active_tab)) {
      $qt_name = $this->name;
      $style = isset($this->options['style']) ? $this->options['style'] : 'nostyle';
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
    
      if (!isset($javascript['setting'][1]['quicktabs']) || !array_key_exists('qt_' . $qt_name, $javascript['setting'][1]['quicktabs'])) {
        $quicktabs_array = array_merge(array('name' => $qt_name, 'tabs' => $tab_settings), $this->options);
        $qt_ra['#attached']['js'][] = array('data' => array('quicktabs' => array('qt_' . $qt_name => $quicktabs_array)), 'type' => 'setting');
      }
    
      if ($this->options['ajax']) {
        $tab = $this->contents[$active_tab];
        $attribs = array(
          'id' => 'quicktabs-tabpage-'. $qt_name . '-'. $active_tab,
          'class' => array('quicktabs-tabpage'),
        );
        $qt_ra['content']['container']['active'] = array(
          '#prefix' => '<div '. drupal_attributes($attribs) .'>',
          '#suffix' => '</div>',
          'content' =>  isset($tab['rendered']) ? $tab['rendered'] : self::render_content_item($tab),
        );
      }
      else {
        // Render all tabpgages.
        $qt_ra['content']['container']['divs'] = array();
        foreach ($this->contents as $key => $tab) {
          $attribs = array(
            'id' => 'quicktabs-tabpage-'. $qt_name . '-'. $key,
            'class' => array('quicktabs-tabpage', ($active_tab == $key ? '' : 'quicktabs-hide')),
          );
          $qt_ra['content']['container']['divs'][] = array(
            '#prefix' => '<div '. drupal_attributes($attribs) .'>',
            '#suffix' => '</div>',
            'content' => isset($tab['rendered']) ? $tab['rendered'] : self::render_content_item($tab),
          );
        }
      }
      return $qt_ra;
    }
    return array();
  }

  
  /**
   * @todo: Enter description here ...
   * @param $active_tab
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

class QuickAccordion extends Quickie {
  
  static $type = 'accordion';
  
  function render() {
    
    // This display style doesn't currently support the ajax option
    $this->options['ajax'] = 0;

    try {
      // Ensures empty tabs are hidden, views args are converted from the url,
      // and custom content won't be attempted to be ajax-rendered.
      $this->prepare_contents();
    }
    catch (InvalidQuickieException $e) {
      return array('#markup' => $e->getMessage());
    }
    
    $active_tab = $this->get_active_tab();
    
    $qt_name = $this->name;
    // Create the quicktabs renderable array and add the necessary css and js to it
    $qt_ra = array(
      '#attached' => array(
        'library' => array(
          array('system', 'ui.accordion'),
        ),
        'js' => array(
          array(
            'data' => "jQuery(function() { jQuery('#quickie-$qt_name').accordion({active: $active_tab})})",
            'type' => 'inline',
          )
        ),
      ),
      'content' => array(
        '#theme' => 'qt_accordion',
        '#options' => array('attributes' => array(
          'id' => 'quickie-' . $qt_name,
          'class' => 'quickie-wrapper',
        )),
      ),
    );
    // Render all tab content.
    $qt_ra['content']['divs'] = array();
    foreach ($this->contents as $key => $item) {
      $qt_ra['content']['divs'][] = array(
        '#prefix' => '<h3><a href= "#">'. $item['title'] .'</a></h3><div>',
        '#suffix' => '</div>',
        'content' => isset($item['rendered']) ? $item['rendered'] : parent::render_content_item($item),
      );
    }
    return $qt_ra;
  }

}

class InvalidQuickieException extends Exception {
  
}