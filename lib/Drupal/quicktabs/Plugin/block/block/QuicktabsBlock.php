<?php

/**
 * @file
 * Contains \Drupal\quicktabs\Plugin\block\block\QuicktabsBlock.
 */

namespace Drupal\quicktabs\Plugin\block\block;

use Drupal\block\BlockBase;
use Drupal\block\Plugin\Core\Entity\Block;
use Drupal\Component\Annotation\Plugin;
use Drupal\Core\Annotation\Translation;
use Drupal\Component\Plugin\Discovery\DiscoveryInterface;

/**
 * Provides a generic Views block.
 *
 * @Plugin(
 *   id = "quicktabs_block",
 *   admin_label = @Translation("Quicktabs Block"),
 *   module = "quicktabs",
 *   derivative = "Drupal\quicktabs\Plugin\Derivative\QuicktabsBlock"
 * )
 */
class QuicktabsBlock extends BlockBase {

  /**
   * The View executable object.
   *
   * @var \Drupal\quicktabs\ViewExecutable
   */
  protected $view;

  /**
   * Overrides \Drupal\Component\Plugin\PluginBase::__construct().
   */
  public function __construct(array $configuration, $plugin_id, DiscoveryInterface $discovery, Block $entity) {
    parent::__construct($configuration, $plugin_id, $discovery, $entity);

    list($plugin, $delta) = explode(':', $this->getPluginId());
    list($name, $this->displayID) = explode('-', $delta, 2);
    // Load the view.
    $this->quicktabs = quicktabs_get_quicktabs($name);
  }

  /**
   * Overrides \Drupal\block\BlockBase::form().
   */
  public function form($form, &$form_state) {
    $form = parent::form($form, $form_state);

    // Set the default label to '' so the quicktabs internal title is used.
    $form['label']['#default_value'] = '';
    $form['label']['#access'] = FALSE;
    return $form;
  }

  /**
   * Implements \Drupal\block\BlockBase::build().
   */
  public function build() {
    $output = $this->quicktabs->render();
    // Set the label to the title configured in the view.
    $this->entity->set('label', filter_xss_admin($this->quicktabs->getTitle()));

    return $output;
  }

}
