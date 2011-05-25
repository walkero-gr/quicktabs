<?php

/***************************************************************************
 * Classes for rendering groups of content, e.g. as tabs
 ***************************************************************************/

/**
 * Factory class for creating Quicktabs instances.
 */
class QuickieFactory {
  
  /**
   * Factory method that creates a Quicktabs instance based on the configuration
   * that is passed to it. See the quicktabs_build_quicktabs() function in
   * quicktabs.module to see how it is used.
   */
  public static function getQuickie($name, $contents, $renderer, $options) {
    // The exact class we return an instance of depends on the renderer parameter
    // that was passed in. Examples are "accordion", "quicktabs" and "ui_tabs".
    $renderer = ucfirst($renderer);
    $func = create_function('$c', 'return strtoupper($c[1]);');
    $type = preg_replace_callback('/_([a-z])/', $func, $renderer);
    
    $class_name = "Quick$type";
    if (class_exists($class_name)) {
      return new $class_name($name, $contents, $options);
    }
  }
}

/**
 * The abstract base class that all Quicktabs renderer classes extend.
 * Provides core functionality that can be used by all renderers.
 */
abstract class Quickie {
  
  /**
   * The unique name of the Quickie object.
   * This corresponds to the machine name as stored in the database
   * or as defined in code.
   * @var string
   */
  protected $name;
  
  /**
   * The contents array.
   * An array defining the "tabs" or contents of the Quickie object.
   * @var array
   */
  protected $contents;
  
  /**
   * The options array.
   * An array of settings controlling the behaviour of the Quickie object, e.g.
   * "ajax" (whether to use ajax), "hide_empty_tabs", default_tabs.
   * TODO: provide a full list of possible options somewhere!
   * @var array
   */
  protected $options;

  /**
   * Constructor
   */
  function __construct($name, $contents, $options) {
    $this->name = $name;
    $this->contents = $contents;
    $this->options = $options;
  }
  
  /**
   * Method to render a Quickie object as a render array.
   */
  abstract function render();

  /**
   * This method does some initial set-up of the "tab" contents, such as hiding
   * tabs with no content if the hide_empty_tabs option is set. It also creates
   * a content builder object for each tab, i.e. an object that implements the
   * QuickContentBuilder inferface. This object is responsible for the actual
   * rendering of the tab contents.
   */
  protected function prepare_contents() {

    if ($this->options['hide_empty_tabs'] && !$this->options['ajax']) {
      // Check if any tabs need to be hidden because of empty content.
      foreach ($this->contents as $key => $tab) {
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $builder = QuickContentFactory::getContentBuilder($tab['type'], $tab);
        $this->contents[$key]['builder'] = $builder;
        $contents = isset($tab['contents']) ? $tab['contents'] : $builder->render_item(TRUE);
        if (empty($contents)) {
          unset($this->contents[$key]);
        }
        else {
          // Since we have rendered the contents already at this point, we should
          // hold on to the rendered output so as not to have to render it again.
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
        $type = $tab['type'];
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $builder = QuickContentFactory::getContentBuilder($type, $tab);
        $this->contents[$key]['builder'] = $builder;
        if ($type == 'custom') {
          if (!$custom) {
            $custom = TRUE;
            $this->options['default_tab'] = $key;
            // Custom tabs already contain their rendered contents. We just need
            // to copy them into the 'rendered' property of the tab.
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
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $builder = QuickContentFactory::getContentBuilder($tab['type'], $tab);
        $this->contents[$key]['builder'] = $builder;
        if ($tab['type'] == 'custom') {
          // Custom tabs already contain their rendered contents. We just need
          // to copy them into the 'rendered' property of the tab.
          $tab['rendered'] = $tab['contents'];
        }
      }
    }
  }
  
  /**
   * Returns the active tab for a given Quicktabs instance. This could be coming
   * from the URL or just from the settings for this instance. If neither, it
   * defaults to 0.
   */
  protected function get_active_tab() {
    $active_tab = isset($this->options['default_tab']) ? $this->options['default_tab'] : key($this->contents);
    $active_tab = isset($_GET['qt-' . $this->name]) ? $_GET['qt-' . $this->name] : $active_tab;
    $active_tab = (isset($active_tab) && isset($this->contents[$active_tab])) ? $active_tab : 0;
    return $active_tab;
  }
  
  /**
   * Static method to retrieve content from an ajax call. This is called by the
   * quicktabs_ajax() callback in quicktabs.module.
   */
  static function ajax_render_content_item($type, $args) {
    $class_name = "Quick{$type}Content";
    if (class_exists($class_name)) {
      $content_builder = new $class_name(array('type' => $type));
      return $content_builder->ajax_render_item($args);
    }
  }
}

/**
 * Renders the content using the jQuery UI Tabs widget.
 */
class QuickUiTabs extends Quickie {
  
  static $type = 'ui_tabs';
  
  function render() {
    
    // This display style doesn't currently support the ajax option
    $this->options['ajax'] = 0;

    try {
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

/**
 * Renders the content using the original Quicktabs mechanism of previous versions.
 * Includes support for ajax rendered content.
 */
class QuickQuicktabs extends Quickie {
  
  // TODO: figure out if I actually need this static var for anything.
  static $type = 'quicktabs';
  
  function render() {

    try {
      $this->prepare_contents();
    }
    catch (InvalidQuickieException $e) {
      return array('#markup' => $e->getMessage());
    }

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
      if (!isset($javascript['setting'][1]['quicktabs']) || !array_key_exists('qt_' . $qt_name, $javascript['setting'][1]['quicktabs'])) {
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

/**
 * Renders the content using the jQuery UI Accordion widget.
 * Includes support for ajax rendered content.
 */
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
          array('data' => drupal_get_path('module', 'quicktabs') . '/js/quicktabs.js'),
        ),
      ),
      'content' => array(
        '#theme' => 'qt_accordion',
        '#options' => array('attributes' => array(
          'id' => 'quickie-' . $qt_name,
          'class' => array('quick-accordion','quickie-wrapper'),
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

/***************************************************************************
 * Classes related to individual tab content.
 ***************************************************************************/

/**
 * Factory class for creating objects that implement the QuickContentBuilder
 * interface.
 */
class QuickContentFactory {
  
  public static function getContentBuilder($type, $item) {
    $type = ucfirst($type);
    $class_name = "Quick{$type}Content";
    if (class_exists($class_name)) {
      return new $class_name($item);
    }
  }
}

/**
 * Interface that any class responsible for rendering individual tab content
 * must implement.
 */
interface QuickContentBuilder {
  /**
   * Method responsible for normal rendering of tab content.
   *
   * @param $hide_empty Used to decide whether, for example, an access denied
   * response for content should be output as such or hidden.
   *
   * @return A render array of the tab content.
   */
  public function render_item($hide_emtpy);
  
  /**
   * Returns an ajax path to be used on ajax-enabled links that retrieve this
   * type of content. The ajax_render_item method (below) essentially reacts to
   * the args of this path.
   *
   * @param $name The machine name of the instance.
   *
   * @param $index The index of the tab, i.e where it fits into the Quickie
   * instance.
   */
  public function get_ajax_path($name, $index);
  
  /**
   * Method responsible for ajax rendering of tab content.
   *
   * @param $args An array of args that were passed to the ajax callback (based on
   * the path returned in get_ajax_path).
   *
   * @return A string of rendered html.
   */
  public function ajax_render_item($args);
}

/**
 * Abstract base class for content builder class, implementing the QuickContentBuilder
 * inteface, providing default implementations of all its methods.
 */
abstract class QuickContent implements QuickContentBuilder {
  
  /**
   * An array containing the information that defines the tab content, e.g.
   * array('type' => 'view', 'vid' => 'myview', 'title' => 'My View').
   * @var array
   */
  protected $item;
  
  /**
   * Constructor
   */
  function __construct($item) {
    $this->item = $item;
  }
  

  public function render_item($hide_empty = FALSE) {
    return array();
  }
  
  public function get_ajax_path($name, $index) {
    return 'quicktabs/ajax/'. $name .'/'. $index . '/'. $this->item['type'];
  }
  public function ajax_render_item($args) {}
  
}

/**
 * Class for tab content of type "qtabs" - this is for rendering a Quickie instance
 * as the tab content of another Quickie instance.
 */
class QuickQtabsContent extends QuickContent {
  public function render_item($hide_empty = FALSE) {
    $item = $this->item;
    $output = array();
    if (isset($item['machine_name'])) {
      if ($quicktabs = quicktabs_load($item['machine_name'])) {
        $contents = $quicktabs->tabs;
        $name = $quicktabs->machine_name;
        unset($quicktabs->tabs, $quicktabs->machine_name);
        $options = (array) $quicktabs;
        $qt = QuickieFactory::getQuickie($name, $contents, $quicktabs->renderer, $options);
        $output = $qt->render();
      }
    }
    return $output;
  }
  
  public function ajax_render_item($args) {
    $item = array();
    $item['machine_name'] = $args[0];
    $this->item = array_merge($this->item, $item);
    $output = $this->render_item();
    return !empty($output) ? drupal_render($output) : '';
  }
  
  public function get_ajax_path($name, $index) {
    $path = parent::get_ajax_path($name, $index);
    return $path . '/'. $this->item['machine_name'];
  }
}

/**
 * Class for tab content of type "view" - this is for rendering a view as tab
 * content.
 */
class QuickViewContent extends QuickContent {
  function __construct($item) {
    //if ($this->options['ajax']) views_add_js('ajax_view');
    views_add_js('ajax_view');
    $url_args = arg();
    $args = $item['args'];

    foreach ($url_args as $id => $arg) {
      $args = str_replace("%$id", $arg, $args);
    }
    $args = preg_replace(',/?(%\d),', '', $args);
    $args = $args ? explode('/', $args) : array();

    $item['args'] = $args;
    $this->item = $item;
  }

  public function render_item($hide_empty = FALSE) {
    $item = $this->item;
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
  
  public function ajax_render_item($args) {
    list($_REQUEST['view_name'], $_REQUEST['view_display_id'], $_REQUEST['view_args']) = $args;
    module_load_include('inc', 'views', 'includes/ajax');
    $view = views_ajax();
    foreach ($view['#commands'] as $command) {
      if ($command['command'] == 'insert') {
        return trim($command['data']);
      }
    }
  }
  
  public function get_ajax_path($name, $index) {
    $path = parent::get_ajax_path($name, $index);
    $path .= '/'. $this->item['vid'] . '/'. $this->item['display'] .'/'. $this->item['args'];
    return $path;
  }
}

/**
 * Class for tab content of type "block" - this is for rendering a block as tab
 * content.
 */
class QuickBlockContent extends QuickContent {
  public function render_item($hide_empty = FALSE) {
    $item = $this->item;
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
  
  public function ajax_render_item($args) {
    $item = array();
    list($item['bid'], $item['hide_title']) = $args;
    $this->item = array_merge($this->item, $item);
    $output = $this->render_item();
    return !empty($output) ? drupal_render($output) : '';
  }
  public function get_ajax_path($name, $index) {
    $path = parent::get_ajax_path($name, $index);
    $path .= '/'. $this->item['bid'];
    return $path;
  }
}

/**
 * Class for tab content of type "node" - this is for rendering a node as tab
 * content.
 */
class QuickNodeContent extends QuickContent {
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


/***********************************************************************/



/**
 * Create our own exception class.
 */
class InvalidQuickieException extends Exception {
  
}