<?php

/***************************************************************************
 * Classes for rendering sets of content, e.g. as tabs
 ***************************************************************************/

/**
 * Factory class for creating QuickSet instances.
 */
class QuickSetFactory {
  
  /**
   * Factory method that creates a QuickSet instance based on the configuration
   * that is passed to it. See the quicktabs_build_quicktabs() function in
   * quicktabs.module to see how it is used.
   */
  public static function getQuickSet($name, $contents, $class, $options) {
    // The exact class we return an instance of depends on the renderer parameter
    // that was passed in. Examples are "accordion", "quicktabs" and "ui_tabs".

    // The next three lines just convert underscore-based names to camel case, e.g.
    // ui_tabs becomes UiTabs.
    //$renderer = ucfirst($renderer);
    //$func = create_function('$c', 'return strtoupper($c[1]);');
    //$type = preg_replace_callback('/_([a-z])/', $func, $renderer);
    
    //$class_name = "Quick$type";
    if (class_exists($class)) {
      return new $class($name, $contents, $options);
    }
    return NULL;
  }
}

/**
 * The abstract base class that all Quicktabs renderer classes extend.
 * Provides core functionality that can be used by all renderers.
 */
abstract class QuickSet {
  
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
   * Constructor
   */
  public function __construct($name, $contents, $options) {
    $this->name = $name;
    $this->contents = $contents;
    $this->options = $options;
  }
  
  /**
   * Method to render a QuickSet object as a render array.
   */
  abstract public function render();

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
            throw new InvalidQuickSetException('The contents specified cannot be rendered via ajax.');
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
   * Method for returning the form elements to display for this tab type on
   * the admin form.
   */
  public function options_form($delta);

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
   * @param $index The index of the tab, i.e where it fits into the QuickSet
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
  
  public function options_form($delta) {
    return array();
  }
  
  /**
   * An array containing the information that defines the tab content, e.g.
   * array('type' => 'view', 'vid' => 'myview', 'title' => 'My View').
   * @var array
   */
  protected $item;
  
  /**
   * Constructor
   */
  public function __construct($item) {
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


/***********************************************************************/



/**
 * Create our own exception class.
 */
class InvalidQuickSetException extends Exception {
  
}