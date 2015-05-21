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
class QuickTabsList extends ControllerBase {
  /**
   * {@inheritdoc}
   */
  public function content(){
    return array('#type' => 'markup', '#markup' => t('Hi all?'),);
  }
}
