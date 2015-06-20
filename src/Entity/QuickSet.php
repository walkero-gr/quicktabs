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
 *       "add" = "Drupal\quicktabs\Form\QuicktabAddForm",
 *       "default" = "Drupal\quicktabs\Form\QuicktabEditForm",
 *       "delete" = "Drupal\quicktabs\Form\QuicktabDeleteForm",
 *       "clone" = "Drupal\quicktabs\Form\QuicktabCloneForm",
 *       "export" = "Drupal\quicktabs\Form\QuicktabExportForm"
 *     },
 *   },
 *   links = {
 *     "add" = "/admin/structure/quicktabs/add",
 *     "edit" = "/admin/structure/quicktabs/manage/{settings}/edit",
 *     "delete" = "/admin/structure/quicktabs/manage/{settings}/delete",
 *     "clone" = "/admin/structure/quicktabs/manage/{settings}/clone",
 *     "export" = "/admin/structure/quicktabs/manage/{settings}/export"
 *   },
 *   entity_keys = {
 *     "id" = "id",
 *     "title" = "title"
 *   },
 *   config_export = {
 *     "id" = "id",
 *     "title" = "title",
 *     "renderer" = "renderer",
 *     "style" = "style",
 *     "ajax" = "ajax",
 *     "hideemptytabs" = "hideemptytabs"
 * },
 * )
 */
class QuickSet extends ConfigEntityBase {

  protected $id;

  protected $title;

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
