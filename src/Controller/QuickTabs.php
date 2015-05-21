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
class QuickTabsList extends ControllerBase {
  /**
   * {@inheritdoc}
   */
  public function content(){
    return array('#type' => 'markup', '#markup' => t('Hi all?'),);
  }
}
