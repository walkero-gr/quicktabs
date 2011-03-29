<?php

class QuickieFactory {
  
  function getQuickie($name, $contents, $renderer, $options) {
    $func = create_function('$c', 'return strtoupper($c[0]);');
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
  
  protected function hide_empty() {
    // Check if any tabs need to be hidden because of empty content.
    if ($this->options['hide_empty_tabs'] && !$this->options['ajax']) {
      // Remove empty tabpages.
      foreach ($this->contents as $key => $tab) {
        $contents = self::render_content_item($tab, TRUE);
        if (empty($contents)) {
          unset($this->contents[$key]);
        }
        else {
          $this->contents[$key]['rendered'] = $contents;
        }
      }
      $this->contents = array_merge($this->contents);
    }
  }
  
  /**
  * Render individual tab content.
  */
  static function render_content_item($item, $hide_empty = FALSE) {
    static $cache;
    $thing = $hide_empty ? 'stuff' : 'nuthin';
    watchdog('qt', $thing);
    $cachekey = md5(serialize($item));
    if (isset($cache[$cachekey])) {
      return $cache[$cachekey];
    }
    $output = array();
    if (isset($item['type'])) {
      $func = 'build_content_'. $item['type'];
      if (is_callable('self', $func)) {
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
    $output = '';
    if (isset($item['bid'])) {
      if (module_exists('block')) {
        $pos = strpos($item['bid'], '_delta_');
        $module = drupal_substr($item['bid'], 0, $pos);
        $delta = drupal_substr($item['bid'], $pos + 7);
  
        $block = block_load($module, $delta);
        $block->region = 'quicktabs_tabpage';
  
        $block_arr = _block_render_blocks(array($block));
        if ($item['hide_title']) {
          $block_arr["{$block->module}_{$block->delta}"]->subject = FALSE;
        }
        if (!empty($block_arr["{$block->module}_{$block->delta}"]->content)) {
          $build = _block_get_renderable_array($block_arr);
          $output = $build;
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

class QuickTabs extends Quickie {
  
  static $type = 'tabs';
  
  function render() {
    // First convert any Views arguments in our tabs to the necessary format
    //$this->contents = _quicktabs_prepare_views_args($this->contents, $this->options['ajax']);
    $this->hide_empty();
  
    $active_tab = isset($options['default_tab']) ? $options['default_tab'] : key($this->contents);
    $active_tab = isset($_GET['qt-' . $this->name]) ? $_GET['qt-' . $this->name] : $active_tab;
    $active_tab = (isset($active_tab) && isset($this->contents[$active_tab])) ? $active_tab : NULL;
    
    
    if ($tabs = $this->build_tablinks($active_tab)) {
      $qt_name = $this->name;
      // Create the quicktabs renderable array and add the necessary css and js to it
      $qt_ra = array(
        '#attached' => array(
          'css' => array(
            //array('data' => drupal_get_path('module', 'quicktabs') .'/css/quicktabs.css'),
          ),
          'js' => array(
            array('data' => drupal_get_path('module', 'quicktabs') . '/js/quicktabs.js'),
            array('data' => 'misc/progress.js', 'weight' => JS_LIBRARY),
            array(
              'data' => 'jQuery(function() { jQuery(".quicktabs_wrapper").quicktabs({qtWrapper : "'. $qt_name .'"})})',
              'type' => 'inline',
            )
          ),
          'library' => array(
            array('system', 'ui.tabs'),
          ),
        ),
        'content' => array(
          '#theme' => 'quicktabs',
          '#options' => array('attributes' => array(
            'id' => 'quicktabs-' . $qt_name,
            'class' => 'quicktabs_wrapper',
          )),
          'tabs' => array('#theme' => 'quicktabs_tabs', '#options' => array('active' => $active_tab), 'tablinks' => $tabs),
        ),
      );
    
      if ($this->options['ajax']) {
        // Render only the active tab content.
        $attribs = array(
          'id' => 'qt-'. $qt_name .'-ui-tabs' . ($active_tab+1),
        );
        $qt_ra['content']['active'] = array(
          '#prefix' => '<div '. drupal_attributes($attribs) .'>',
          '#suffix' => '</div>',
          'content' =>  self::render_content_item($this->contents[$active_tab]),
        );
      }
      else {
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
    //$output = '';
    $tabs_count = count($this->contents);
    if ($tabs_count <= 0) {
      return NULL;
    }
    $qt_name = $this->name;
    foreach ($this->contents as $i => $tab) {
      $active = $i == $active_tab;
      $class[] = 'qtab-' . $i;
      // Add first, last and active classes to the list of tabs to help out themers.
      if ($i == 0) {
        $class[] = 'first';
      }
      if ($i == $tabs_count-1) {
        $class[] = 'last';
      }
      $type = $tab['type'];
      $type_ids = array('block' => 'bid', 'node' => 'nid', 'view' => 'vid', 'qtabs' => 'machine_name');
      $ajax_href = 'qt/nojs/'. $type .'/'. $tab[$type_ids[$type]] .'/'. $qt_name .'/'. ($i + 1);
      // Construct our render array for the link.
      $tablink = array(
        '#type' => 'link',
        '#title' => $tab['title'],
        '#href' => $this->options['ajax'] && !$active ? $ajax_href : '#',
        '#options' => $this->_quicktabs_construct_link_options($i),
      );
      if ($this->options['ajax'] && !$active) {
        $tablink['#ajax'] = array(
          'wrapper' => 'qt-'. $qt_name .'-ui-tabs'. ($i + 1),
          'method' => 'html',
          'bindMethod' => 'one',
          'progress' => array('type' => 'throbber', 'message' => NULL),
        );
      }
      $tabs[] = $tablink;
    }
  
    return $tabs;
  }
  
  /**
   * Helper function to construct link options for tab links.
   */
  protected function _quicktabs_construct_link_options($tabkey) {
    $qt_name = $this->name;
    $ajax = $this->options['ajax'];
    $tab = $this->contents[$tabkey];
    $id = 'quicktabs-tab-' . implode('-', array($qt_name, $tabkey));
    $dest = $_GET['q'];
    // Need to construct the correct querystring for the tab links.
    $query = drupal_get_query_parameters(NULL, array("qt-$qt_name", 'q', 'page'));
    $query["qt-{$qt_name}"] = $tabkey;
    $query['dest'] = $dest;
    $link_options = array(
      'attributes' => array(
        'id' => $id,
      ),
      'query' => $query,
      'fragment' => "qt-" . $qt_name . "-ui-tabs". ($tabkey+1),
    );
    return $link_options;
  }
}

class QuickAccordion extends Quickie {
  
  static $type = 'accordion';
  
  function render() {
    // First convert any Views arguments in our tabs to the necessary format
    //$this->contents = _quicktabs_prepare_views_args($this->contents, $this->options['ajax']);

    // Check if any tabs need to be hidden because of empty content.
    if ($this->options['hide_empty_tabs'] && !$this->options['ajax']) {
      // Remove empty tabpages.
      foreach ($this->contents as $key => $tab) {
        $contents = parent::render_content_item($tab, TRUE);
        if (empty($contents)) {
          unset($this->contents[$key]);
        }
        else {
          $this->contents[$key]['rendered'] = $contents;
        }
      }
      $this->contents = array_merge($this->contents);
    }
    
    $qt_name = $this->name;
    // Create the quicktabs renderable array and add the necessary css and js to it
    $qt_ra = array(
      '#attached' => array(
        'library' => array(
          array('system', 'ui.accordion'),
        ),
        'js' => array(
          array(
            'data' => 'jQuery(function() { jQuery("#quickie-'. $qt_name .'").accordion()})',
            'type' => 'inline',
          )
        ),
      ),
      'content' => array(
        '#theme' => 'quick_accordion',
        '#options' => array('attributes' => array(
          'id' => 'quickie-' . $qt_name,
          'class' => 'quickie_wrapper',
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