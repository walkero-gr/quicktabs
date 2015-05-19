<?php

namespace Drupal\quicktabs\Controller;

/**
 * Class QuickTabsList
 */
class QuickTabsList {
  /**
   * @return array
   */
  public function description(){
    return array('#type' => 'markup', '#markup' => t('Hi all?'),);
    
  }
}
