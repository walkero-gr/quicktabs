<?php

/**
 * @file
 * Contains \Drupal\quicktabs\QuicktabsListController.
 */

namespace Drupal\quicktabs;

use Drupal\Core\Config\Entity\ConfigEntityListController;
use Drupal\Core\Entity\EntityInterface;

/**
 * List controller for the shortcut set entity edit forms.
 */
class QuicktabsListController extends ConfigEntityListController {
  /**
   * Overrides \Drupal\Core\Entity\EntityListController::buildHeader().
   */
  public function buildHeader() {
    $row['label'] = t('Name');
    $row['operations'] = t('Operations');
    return $row;
  }

  /**
   * Overrides \Drupal\Core\Entity\EntityListController::getOperations().
   */
  public function getOperations(EntityInterface $entity) {
    $uri = $entity->uri();
    $operations['edit'] = array(
      'title' => t('Edit set'),
      'href' => $uri['path'] . '/edit',
      'options' => $uri['options'],
      'weight' => 10,
    );
    $operations['delete'] = array(
      'title' => t('Delete set'),
      'href' => $uri['path'] . '/delete',
      'options' => $uri['options'],
      'weight' => 100,
    );

    return $operations;
  }

  /**
   * Overrides \Drupal\Core\Entity\EntityListController::buildRow().
   */
  public function buildRow(EntityInterface $entity) {
    $row['name'] = check_plain($entity->label());
    $row['operations']['data'] = $this->buildOperations($entity);
    return $row;
  }

}
