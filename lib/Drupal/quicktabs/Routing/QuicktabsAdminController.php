<?php

/**
 * @file
 * Contains \Drupal\quicktabs\Routing\QuicktabsAdminController.
 */

namespace Drupal\quicktabs\Routing;

use Drupal\Core\ControllerInterface;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityManager;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Ajax\AjaxResponse;
use Drupal\quicktabs\Plugin\Core\Entity\Quicktabs;

/**
 * Returns responses for Quicktabs routes.
 */
class QuicktabsAdminController implements ControllerInterface {

  /**
   * Stores the Entity manager.
   *
   * @var \Drupal\Core\Entity\EntityManager
   */
  protected $entityManager;

  /**
   * Constructs a new \Drupal\views_ui\Routing\ViewsUIController object.
   *
   * @param \Drupal\Core\Entity\EntityManager $entity_manager
   *   The Entity manager.
   * @param \Drupal\views\ViewsDataCache views_data
   *   The Views data cache object.
   * @param \Drupal\user\TempStoreFactory $temp_store_factory
   *   The factory for the temp store object.
   */
  public function __construct(EntityManager $entity_manager) {
    $this->entityManager = $entity_manager;
  }

  /**
   * Implements \Drupal\Core\ControllerInterface::create().
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('plugin.manager.entity')
    );
  }

  /**
   * Lists all of the views.
   *
   * @return array
   *   The Views listing page.
   */
  public function listing() {
    return $this->entityManager->getListController('quicktabs')->render();
  }

  /**
   * Returns the form to add a new view.
   *
   * @return array
   *   The Views add form.
   */
  public function add() {
    drupal_set_title(t('Add new Quicktabs Instance'));

    $entity = $this->entityManager->getStorageController('quicktabs')->create(array());
    return entity_get_form($entity, 'add');
  }

  /**
   * Returns the form to add a new view.
   *
   * @return array
   *   The Views add form.
   */
  public function edit(Quicktabs $quicktabs) {
    $name = $quicktabs->get('label');
    drupal_set_title($name);
    return entity_get_form($quicktabs, 'edit');

  }

  /**
   * Returns the form to clone a view.
   *
   * @param \Drupal\views\ViewStorageInterface $view
   *   The view being cloned.
   *
   * @return array
   *   The Views clone form.
   */
  public function cloneForm(ViewStorageInterface $view) {
    drupal_set_title(t('Clone of @human_name', array('@human_name' => $view->getHumanName())));
    return entity_get_form($view, 'clone');
  }


}
