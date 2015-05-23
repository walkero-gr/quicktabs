<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Controller\QuickTabsList.
 */

namespace Drupal\quicktabs\Controller;

use Drupal\Core\Controller\ControllerBase;
  /**
 * Class QuickTabsList
 */
class QuickTabsListController extends ControllerBase {
  /**
   * {@inheritdoc}
   */
  public function content() {
    $build = array(
      '#type' => 'markup',
      '#markup' => t('Hello World'),
    );
  }
}