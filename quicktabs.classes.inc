<?php

/**
 * A QuickSet object is an unrendered Quicktabs intance, as defined by a set of options
 * and the contents it contains.
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
   * Returns an ajax path to be used on ajax-enabled links.
   *
   * @param $index The index of the tab, i.e where it fits into the QuickSet
   * instance.
   *
   * @param $type The type of content we are providing an ajax path for.
   */
  public function getAjaxPath($index, $type) {
    return 'quicktabs/ajax/'. $this->name .'/'. $index . '/'. $type;
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
        $renderer = self::content_renderer_factory($tab);
        if (!$renderer) {
          unset($this->contents[$key]);
        }
        else {
          $this->contents[$key]['renderer'] = $renderer;
          $contents = $renderer->render(TRUE);
          if (empty($contents)) {
            unset($this->contents[$key]);
          }
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
      $has_prerendered = FALSE; // keep track of whether we have found a custom tab.
      $invalid = FALSE; // Flag for if there is more than one custom tab.
      foreach ($this->contents as $key => $tab) {
        $type = $tab['type'];
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $renderer = self::content_renderer_factory($tab);
        if (!$renderer) {
          $invalid = TRUE;
          break;
        }
        $this->contents[$key]['renderer'] = $renderer;
        if ($type == 'prerendered') {
          if (!$has_prerendered) {
            $has_prerendered = TRUE;
            $this->options['default_tab'] = $key;
          }
          else {
            // We are on a second custom tab and the ajax option is set, we cannot
            // render custom tabs via ajax, so we skip out of the loop, set the
            // ajax option to off, and call the method again.
            $invalid = TRUE;
            break;
          }
        }
      }
      if ($invalid) {
        // The contents are invalid for ajax mode, so we set the ajax option to
        // false and prepare contents again.
        $this->options['ajax'] = 0;
        $this->prepare_contents();
        return;
      }
    }
    else {
      foreach ($this->contents as $key => $tab) {
        // Instantiate a content builder object and add a reference to it
        // in the tab array.
        $renderer = self::content_renderer_factory($tab);
        $this->contents[$key]['renderer'] = $renderer;
        if (!$renderer)  {
          throw new InvalidQuickSetException(t('Content of type @type cannot be rendered in a tab. Please check that all plugin dependencies are enabled', array('@type' => $tab['type'])));
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
  
  public static function content_renderer_factory($tab) {
    if ($tab['type'] == 'prerendered') {
      return new QuickPreRenderedContent($tab);
    }
    return QuickContent::factory($tab['type'], $tab);
  }
  
  /**
   * Static method to retrieve content from an ajax call. This is called by the
   * quicktabs_ajax() callback in quicktabs.module.
   */
  public static function ajax_render_content_item($type, $args) {
    if ($renderer = self::content_renderer_factory(array('type' => $type))) {
      $output = $renderer->render(FALSE, $args);
      return !empty($output) ? drupal_render($output) : '';
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

interface QuickContentRenderable {
  
  public function render($hide_empty = FALSE, $args = array());
  
  public function getAjaxArgs();
  
}

/**
 * Abstract base class for individual content items.
 */
abstract class QuickContent implements QuickContentRenderable {

  /**
   * An array containing the information that defines the tab content, e.g.
   * array('type' => 'view', 'vid' => 'myview', 'title' => 'My View').
   * @var array
   */
  protected $item;
  
  
  /**
   * A render array of the contents.
   */
  protected $rendered_content;

  /**
   * Instantiate a content type object.
   *
   * @param $name
   *   The type name of the plugin.
   *
   * @param $item
   *   An array containing the item definition
   *  
   */
  public static function factory($name, $item) {
    ctools_include('plugins');
    if ($class = ctools_plugin_load_class('quicktabs', 'contents', $name, 'handler')) {
      // We now need to check the plugin's dependencies, to make sure they're installed.
      // This info has already been statically cached at this point so there's no
      // harm in making a call to ctools_get_plugins().
      $plugin = ctools_get_plugins('quicktabs', 'contents', $name);
      if (isset($plugin['dependencies'])) {
        foreach ($plugin['dependencies'] as $dep) {
          // Do not load the options form for any plugin that is missing dependencies.
          if (!module_exists($dep)) return NULL;
        }
      }
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
  public function render($hide_empty = FALSE, $args = array()) {
    return array();
  }
  
  /**
   * Returns an array of arguments that are used to construct the full ajax
   * path that should be called to get the content via ajax.
   */
  public function getAjaxArgs() {
    return array();
  }
  
}

class QuickPreRenderedContent implements QuickContentRenderable {

  /**
   * An array containing the information that defines the tab content, e.g.
   * array('type' => 'view', 'vid' => 'myview', 'title' => 'My View').
   * @var array
   */
  protected $item;
  
  /**
   * A render array of the contents.
   */
  protected $rendered_content;

  /**
   * Constructor
   */  
  public function __construct($item) {
    $this->item = $item;
    $contents = isset($item['contents']) ? $item['contents'] : array();
    if (!is_array($contents)) {
      $contents = array('#markup' => $contents);
    }
    $this->rendered_content = $contents;
  }
  
  public function render($hide_empty = FALSE, $args = array()) {
    return $this->rendered_content;
  }
  
  public function getAjaxArgs() {
    return array();
  }
}

/**
 * Create our own exception class.
 */
class InvalidQuickSetException extends Exception {
  
}