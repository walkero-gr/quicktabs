<?php

/**
 * @file
 * Contains \Drupal\quicktabs\Plugin\Derivative\QuicktabsBlock.
 */

namespace Drupal\quicktabs\Plugin\Derivative;

use Drupal\Component\Plugin\Derivative\DerivativeInterface;

/**
 * Provides block plugin definitions for all Quicktabs block displays.
 *
 * @see \Drupal\quicktabs\Plugin\block\block\QuicktabsBlock
 */
class QuicktabsBlock implements DerivativeInterface {

  /**
   * List of derivative definitions.
   *
   * @var array
   */
  protected $derivatives = array();

  /**
   * Implements \Drupal\Component\Plugin\Derivative\DerivativeInterface::getDerivativeDefinition().
   */
  public function getDerivativeDefinition($derivative_id, array $base_plugin_definition) {
    if (!empty($this->derivatives) && !empty($this->derivatives[$derivative_id])) {
      return $this->derivatives[$derivative_id];
    }
    $this->getDerivativeDefinitions($base_plugin_definition);
    return $this->derivatives[$derivative_id];
  }

  /**
   * Implements \Drupal\Component\Plugin\Derivative\DerivativeInterface::getDerivativeDefinitions().
   */
  public function getDerivativeDefinitions(array $base_plugin_definition) {
    // Check all Quicktabs for block displays.
    foreach (quicktabs_get_all_quicktabs() as $qt) {
      $delta = $qt->id();

      $desc = t('Quicktabs block: !title', array('!title' => $qt->get('title')));
      $this->derivatives[$delta] = array(
        'admin_label' => $desc,
      );
      $this->derivatives[$delta] += $base_plugin_definition;

    }
    return $this->derivatives;
  }

}
