<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Form\QuicktabEditForm.php
 */
namespace Drupal\quicktabs\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class QuicktabEditForm
 *
 */
class QuicktabEditForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'quicktab edit';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state ){
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }
}
