<?php
/**
 * @file
 * Contains \Drupal\quicktabs\QuicktabListBuilder.
 */

namespace Drupal\quicktabs;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Config\Entity\ConfigEntityListBuilder;

/**
 * Class QuicktabListBuilder
 *
 * @see \Drupal\quicktabs\Entity\QuickSet
 */
class QuicktabListBuilder extends ConfigEntityListBuilder {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'quicktabs_list';
  }

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['title'] = $this->t('QuickSet');
    $header['storage'] = $this->t('Storage');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['title'] = $entity->id();
    $row['storage'] = $this->t('Normal');
    return $row + parent::buildRow($entity);
  }

  /**
   * {@inheritdoc}
   */
  public function getDefaultOperations(EntityInterface $entity) {
    $operations = parent::getDefaultOperations($entity);

    /*if ($entity->hasLinkTemplate('edit')) {
      $operations['edit'] = array(
        'title' => t('Edit quicktab'),
        'weight' => 20,
        'url' => $entity->urlInfo('edit'),
      );
      drupal_set_message($this->t('Hi'));
    }*/
    return $operations;
  }
}
