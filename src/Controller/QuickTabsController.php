<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Controller\QuickTabs.
 */

namespace Drupal\quicktabs\Controller;

use Drupal\Core\Controller\ControllerBase;
/**
 * Class QuickTabs
 */
class QuickTabs extends ControllerBase {
  /**
   * {@inheritdoc}
   */
  public function content() {
    $build = array(
      '#type' => 'markup',
      '#markup' => t('Hello World'),
    );
    return $build;
  }
}
