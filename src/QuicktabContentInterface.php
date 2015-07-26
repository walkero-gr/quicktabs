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
   * @return array
   */
  public function optionsForm();

  /**
   * @return array
   */
  public function render();

  /**
   * @return array
   */
  public function getAjaxKeys();

  /**
   * @return array
   */
  public function getUniqueKeys();
}