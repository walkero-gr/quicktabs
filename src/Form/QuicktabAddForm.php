<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Form\QuicktabAddForm.php
 */
namespace Drupal\quicktabs\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Class QuicktabAddForm
 *
 */
class QuicktabAddForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'quicktab add';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state ){
/*  $config = \Drupal::config('quicktabs.add');
    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('title'),
      '#default_value' => $config->get('title'),
    ); */
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }
}
