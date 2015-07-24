<?php
/**
 * @file
 * Contains Drupal\src\QuicktabsManager.php
 */

namespace Drupal\quicktabs;

use \Drupal\Core\Plugin\DefaultPluginManager;
use \Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Quicktabs Plugin Manager.
 */
class QuicktabsManager extends DefaultPluginManager {

  /**
   * Contructs an Quicktab object
   */
  public function __construct(\Traversable $namespaces, ModuleHandlerInterface $module_handler) {
    parent::__construct('Plugin', $module_handler, 'Drupal\quicktabs\QuicktabsInterface', 'Drupal\quicktabs\Annotation');
  }
}