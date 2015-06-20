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
use Drupal\Component\Utility\SafeMarkup;

/**
 * Class QuicktabListBuilder
 *
 * @see \Drupal\quicktabs\Entity\QuickSet
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
    $header['title'] = $this->t('Name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['title']['#markup'] = SafeMarkup::checkPlain($entity->id());
    return $row + parent::buildRow($entity);
  }

}
