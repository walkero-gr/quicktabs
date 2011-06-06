<?php

/**
 * The class responsible for creating Quicktabs instances.
 */
class QuickSet {
  
  /**
   * The unique name of the QuickSet object.
   * This corresponds to the machine name as stored in the database
   * or as defined in code.
   * @var string
   */
  protected $name;
  
  /**
   * The contents array.
   * An array defining the "tabs" or contents of the QuickSet object.
   * @var array
   */
  protected $contents;
  
  /**
   * The options array.
   * An array of settings controlling the behaviour of the QuickSet object, e.g.
   * "ajax" (whether to use ajax), "hide_empty_tabs", default_tabs.
   * TODO: provide a full list of possible options somewhere!
   * @var array
   */
  protected $options;
  

  /**
   * Accessors.
   */
  
  public function getName() {
    return $this->name;
  }
  
  public function getContents() {
    return $this->contents;
  }
  
  public function getOptions() {
    return $this->options;
  }

  /**
   * Instantiate, populate and return a QuickSet object wrapped in a renderer.
   *
   * @param $name
   *   The unique name (machine name) of the QuickSet instance.
   *
   * @param $contents
   *   The array of content items, each one itself an array with at least a 'type'
   *   key, a title key, and the other info necessary for that type.
   *
   * @param $renderer
   *   The plugin key for this renderer plugin
   *
   * @param $options
   *   An array of options determining the behaviour of this QuickSet instance.
   *  
   */
  public static function rendered_quickset_factory($name, $contents, $renderer, $options) {
    ctools_include('plugins');
    if ($class = ctools_plugin_load_class('quicktabs', 'renderers', $renderer, 'handler')) {
      try {
        $qs = new self($name, $contents, $options);
      }
      catch (InvalidQuickSetException $e) {
        watchdog('Quicktabs', $e->getMessage());
        return NULL;
      }
      return new $class($qs);
    }
  }
  
  /**
   * Constructor
   */
  public function __construct($name, $contents, $options) {
    $this->name = $name;
    $this->contents = $contents;
    $this->options = $options;
    $this->prepare_contents();
  }

  /**
   * This method does some initial set-up of the "tab" contents, such as hiding
   * tabs with no content if the hide_empty_tabs option is set. It also creates
   * a content builder object for each tab, i.e. an object that implements the
   * QuickContentBuilder inferface. This object is responsible for the actual
   * rendering of the tab contents.
   */
  protected function prepare_contents() {
    if (!count($this->contents))  {
      throw new InvalidQuickSetException('There are no contents to render.');
    }
    if ($this->options['hide_empty_tabs'] && !$this->options['ajax']) {
      // Check if any tabs need to be hidden because of empty content.
      foreach ($this->contents as $key => $tab) {
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $builder = QuickContent::factory($tab['type'], $tab);
        $this->contents[$key]['builder'] = $builder;
        $contents = isset($tab['contents']) ? $tab['contents'] : $builder->render_item(TRUE);
        if (empty($contents)) {
          unset($this->contents[$key]);
        }
        else {
          // Since we have rendered the contents already at this point, we should
          // hold on to the rendered output so as not to have to render it again.
          $this->contents[$key]['rendered'] = is_array($contents) ? $contents : array('#markup' => $contents);
        }
      }
      $this->contents = array_merge($this->contents);
      if (!count($this->contents))  {
        throw new InvalidQuickSetException('There are no contents to render.');
      }
    }
    elseif ($this->options['ajax']) {
      // Make sure that there is at most 1 "custom" tab and it is the default tab.
      // Custom content cannot be rendered via ajax.
      $custom = FALSE; // keep track of whether we have found a custom tab.
      $custom_ajax_error = FALSE; // Flag for if there is more than one custom tab.
      foreach ($this->contents as $key => &$tab) {
        $type = $tab['type'];
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $builder = QuickContent::factory($type, $tab);
        $this->contents[$key]['builder'] = $builder;
        if ($type == 'custom') {
          if (!$custom) {
            $custom = TRUE;
            $this->options['default_tab'] = $key;
            // Custom tabs already contain their rendered contents. We just need
            // to copy them into the 'rendered' property of the tab.
            $tab['rendered'] = is_array($tab['contents']) ? $tab['contents'] : array('#markup' => $tab['contents']);
          }
          else {
            // We are on a second custom tab and the ajax option is set, we cannot
            // render custom tabs via ajax, so we skip out of the loop, set the
            // ajax option to off, and call the method again.
            $custom_ajax_error = TRUE;
            break;
          }
        }
      }
      if ($custom_ajax_error) {
        $this->options['ajax'] = 0;
        $this->prepare_contents();
        return;
      }
    }
    else {
      // Copy "contents" into the "rendered" property for custom tabs.
      foreach ($this->contents as $key => &$tab) {
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $builder = QuickContent::factory($tab['type'], $tab);
        $this->contents[$key]['builder'] = $builder;
        if ($tab['type'] == 'custom') {
          // Custom tabs already contain their rendered contents. We just need
          // to copy them into the 'rendered' property of the tab.
          $tab['rendered'] = is_array($tab['contents']) ? $tab['contents'] : array('#markup' => $tab['contents']);
        }
      }
    }
    // Set the default style if necessary.
    if ($this->options['style'] == 'default') $this->options['style'] = variable_get('quicktabs_tabstyle', 'nostyle');
  }
  
  /**
   * Returns the active tab for a given Quicktabs instance. This could be coming
   * from the URL or just from the settings for this instance. If neither, it
   * defaults to 0.
   */
  public function get_active_tab() {
    $active_tab = isset($this->options['default_tab']) ? $this->options['default_tab'] : key($this->contents);
    $active_tab = isset($_GET['qt-' . $this->name]) ? $_GET['qt-' . $this->name] : $active_tab;
    $active_tab = (isset($active_tab) && isset($this->contents[$active_tab])) ? $active_tab : 0;
    return $active_tab;
  }
  
  /**
   * Static method to retrieve content from an ajax call. This is called by the
   * quicktabs_ajax() callback in quicktabs.module.
   */
  public static function ajax_render_content_item($type, $args) {
    if ($builder = QuickContent::factory($type, array('type' => $type))) {
      return $builder->ajax_render_item($args);
    }
    return '';
  }
}

/**
 * Interface that all renderer plugins must implement.
 */
abstract class QuickRenderer {
  
  protected $quickset;
  
  public function __construct($quickset) {
    $this->quickset = $quickset;
  }
  
  abstract public function render();
  
  protected function add_attached() {
    return array();
  }
  
  protected function build_tablinks() {
    return array();
  }
  
}

/**
 * Abstract base class for individual content items.
 */
abstract class QuickContent {

  /**
   * An array containing the information that defines the tab content, e.g.
   * array('type' => 'view', 'vid' => 'myview', 'title' => 'My View').
   * @var array
   */
  protected $item;

  /**
   * Instantiate, populate and return a QuickSet object.
   *
   * @param $renderer
   *
   * @param $name
   *   The unique name (machine name) of the QuickSet instance.
   *
   * @param $contents
   *   The array of content items, each one itself an array with at least a 'type'
   *   key, a title key, and the other info necessary for that type.
   *
   * @param $renderer
   *   The plugin key for this renderer plugin
   *
   * @param $options
   *   An array of options determining the behaviour of this QuickSet instance.
   *  
   */
  public static function factory($name, $item) {
    ctools_include('plugins');
    if ($class = ctools_plugin_load_class('quicktabs', 'contents', $name, 'handler')) {
      $content = new $class($item);
      return $content;
    }
    return NULL;
  }

  /**
   * Constructor
   */
  public function __construct($item) {
    $this->item = $item;
  }

  /**
   * Method for returning the form elements to display for this tab type on
   * the admin form.
   */  
  abstract public function options_form($delta);
  
  /**
   * Method responsible for normal rendering of tab content.
   *
   * @param $hide_empty Used to decide whether, for example, an access denied
   * response for content should be output as such or hidden.
   *
   * @return A render array of the tab content.
   */
  abstract public function render_item($hide_empty);

  /**
   * Returns an ajax path to be used on ajax-enabled links that retrieve this
   * type of content. The ajax_render_item method (below) essentially reacts to
   * the args of this path.
   *
   * @param $name The machine name of the instance.
   *
   * @param $index The index of the tab, i.e where it fits into the QuickSet
   * instance.
   */
  public function get_ajax_path($name, $index) {
    return 'quicktabs/ajax/'. $name .'/'. $index . '/'. $this->item['type'];
  }
  
  /**
   * Method responsible for ajax rendering of tab content.
   *
   * @param $args An array of args that were passed to the ajax callback (based on
   * the path returned in get_ajax_path).
   *
   * @return A string of rendered html.
   */
  abstract public function ajax_render_item($args);
  
}


/**
 * Create our own exception class.
 */
class InvalidQuickSetException extends Exception {
  
}