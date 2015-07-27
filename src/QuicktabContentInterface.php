<?php
/**
 * @file
 * Contains \Drupal\quicktabs\QuickTabContentInterface.
 */

namespace Drupal\quicktabs;

/**
 * Helper interface to other Content Plugins.
 */
interface QuicktabContentInterface {

  /**
   * @return string
   */
  public static function getType();

  /**
   * @param int $delta
   * @param \Drupal\quicktabs\An $qt
   * @return array
   */
  public function optionsForm($delta, $qt);

  /**
   * @param bool $hide_empty
   * @param array $args
   * @return array
   */
  public function render($hide_empty = FALSE, $args = array());

  /**
   * @return array
   */
  public function getAjaxKeys();

  /**
   * @return array
   */
  public function getUniqueKeys();
}