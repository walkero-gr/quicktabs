<?php

/**
 * @file
 * Definition of Drupal\quicktabs\Plugin\Core\Entity\Quicktabs.
 */

namespace Drupal\quicktabs\Plugin\Core\Entity;

use Drupal\Core\Config\Entity\ConfigStorageController;
use Drupal\Core\Config\Entity\ConfigEntityBase;
use Drupal\Core\Config\Entity\ConfigEntityInterface;
use Drupal\Component\Annotation\Plugin;
use Drupal\Core\Annotation\Translation;

/**
 * Defines a Quicktabs configuration entity class.
 *
 * @Plugin(
 *   id = "quicktabs",
 *   label = @Translation("Quicktabs"),
 *   module = "quicktabs",
 *   controller_class = "Drupal\Core\Config\Entity\ConfigStorageController",
 *   list_controller_class = "Drupal\quicktabs\QuicktabsListController",
 *   form_controller_class = {
 *     "add" = "Drupal\quicktabs\QuicktabsFormController",
 *     "edit" = "Drupal\quicktabs\QuicktabsFormController",
 *   },
 *   config_prefix = "quicktabs",
 *   fieldable = FALSE,
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid",
 *   }
 * )
 */
class Quicktabs extends ConfigEntityBase {

  /**
   * The unique ID of the Quicktabs instance.
   *
   * @var string
   */
  public $id = NULL;


  /**
   * The human readable name of the Quicktabs instance.
   *
   * @var string
   */
  public $title = '';

  /**
   * The UUID for this entity.
   *
   * @var string
   */
  public $uuid = NULL;

  /**
   * Overrides Drupal\Core\Entity\EntityInterface::uri().
   */
  public function uri() {
    return array(
      'path' => 'admin/structure/quicktabs/manage/' . $this->id(),
      'options' => array(
        'entity_type' => $this->entityType,
        'entity' => $this,
      ),
    );
  }

  /**
   * Overrides \Drupal\Core\Config\Entity\ConfigEntityBase::getExportProperties();
   */
  public function getExportProperties() {
    $names = array(
      'id',
      'label',
      'hide_empty_tabs',
      'default_tab',
      'uuid',
      'tabs',
      'renderer',
      'options',
    );
    $properties = array();
    foreach ($names as $name) {
      $properties[$name] = $this->get($name);
    }
    return $properties;
  }

}
