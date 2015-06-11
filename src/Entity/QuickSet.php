<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Entity\QuickSet.
 */

namespace Drupal\quicktabs\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;


/**
 * Defines the QuickSet entity.
 *
 * The QuickSet entity stores information about a quicktab.
 *
 * @ConfigEntityType(
 *   id = "settings",
 *   label = @Translation("Settings"),
 *   module = "quicktabs",
 *   config_prefix = "settings",
 *   admin_permission = "administer quicktabs",
 *   handlers = {
 *     "storage" = "Drupal\quicktabs\QuicktabStorage",
 *     "list_builder" = "Drupal\quicktabs\QuicktabListBuilder",
 *     "form" = {
 *       "default" = "Drupal\quicktabs\Form\QuicktabEditForm",
 *       "delete" = "Drupal\quicktabs\Form\QuicktabDeleteForm",
 *       "clone" = "Drupal\quicktabs\Form\QuicktabCloneForm"
 *     },
 *   },
 *   links = {
 *     "edit-form" = "/admin/structure/quicktabs/manage/{quicktabs}/edit",
 *     "delete-form" = "/admin/structure/quicktabs/manage/{quicktabs}/delete",
 *     "clone-form" = "/admin/structure/quicktabs/manage/{quicktabs}/clone"
 *   },
 *   entity_keys = {
 *     "title" = "title",
 *     "machine_name" = "machine_name",
 *     "renderer" = "renderer",
 *     "style" = "style",
 *     "ajax" = "ajax",
 *     "hideemptytabs" = "hideemptytabs"
 *   },
 *   config_export = {
 *     "title" = "title",
 *     "machine_name" = "machine_name",
 *     "renderer" = "renderer",
 *     "style" = "style",
 *     "ajax" = "ajax",
 *     "hideemptytabs" = "hideemptytabs"
 * },
 * )
 */
class QuickSet extends ConfigEntityBase {

  protected $title;

  protected $machine_name;

  protected $renderer;

  protected $style;

  protected $ajax;

  protected $hideemptytabs;

  public function getTitle() {
    return $this->title;
  }

  public function getRenderer() {
    return $this->renderer;
  }

  public function getStyle() {
    return $this->style;
  }

  public function isAjax() {
    return $this->ajax;
  }

  public function getHideEmptyTabs() {
    return $this->hideemptytabs;
  }
}