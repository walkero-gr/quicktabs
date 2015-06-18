<?php
/**
 * @file
 * Contains \Drupal\quicktabs\QuicktabListBuilder.
 */

namespace Drupal\quicktabs;

use Drupal\Core\Config\Entity\DraggableListBuilder;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Config\Entity\ConfigEntityListBuilder;

/**
 * Class QuicktabListBuilder
 */
class QuicktabListBuilder extends ConfigEntityListBuilder {

  /**
   * {@inheritdoc]
   */
  public function getFormId() {
    return 'quicktabs_list';
  }

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['label'] = $this->getLabel($entity);
    return $row + parent::buildRow($entity);
  }

}
