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
    $form['title'] = array(
      '#title' => t('Title'),
      '#description' => t('This will appear as the block title.'),
      '#type' => 'textfield',
     // '#default_value' => isset($qt->title) ? $qt->title : '',
      '#weight' => -9,
      '#required' => TRUE,
    );

    $form['machine_name'] = array(
      '#type' => 'machine_name',
      '#maxlength' => 32,
      '#machine_name' => array(
      //  'exists' => 'quicktabs_machine_name_exists',
        'source' => array('title'),
      ),
      '#description' => t('A unique machine-readable name for this Quicktabs instance. It must only contain lowercase letters, numbers, and underscores. The machine name will be used internally by Quicktabs and will be used in the CSS ID of your Quicktabs block.'),
      '#weight' => -8,
    );

    $renderer_options = '';
    $form['renderer'] = array(
      '#type' => 'select',
      '#title' => t('Renderer'),
      '#options' => $renderer_options,
      //'#default_value' => isset($qt->renderer) ? $qt->renderer : 'quicktabs',
      '#description' => t('Choose how to render the content.'),
      '#weight' => -7,
    );

    $form['style'] = array(
      '#type' => 'value',
      '#value' => 'nostyle',
    );

    $form['ajax'] = array(
      '#type' => 'radios',
      '#title' => t('Ajax'),
      '#options' => array(
        TRUE => t('Yes') . ': ' . t('Load only the first tab on page view'),
        FALSE => t('No') . ': ' . t('Load all tabs on page view.'),
      ),
      '#default_value' => isset($qt->ajax) ? $qt->ajax : 0,
      '#description' => t('Choose how the content of tabs should be loaded.<p>By choosing "Yes", only the first tab will be loaded when the page first viewed. Content for other tabs will be loaded only when the user clicks the other tab. This will provide faster initial page loading, but subsequent tab clicks will be slower. This can place less load on a server.</p><p>By choosing "No", all tabs will be loaded when the page is first viewed. This will provide slower initial page loading, and more server load, but subsequent tab clicks will be faster for the user. Use with care if you have heavy views.</p><p>Warning: if you enable Ajax, any block you add to this quicktabs block will be accessible to anonymous users, even if you place role restrictions on the quicktabs block. Do not enable Ajax if the quicktabs block includes any blocks with potentially sensitive information.</p>'),
      '#states' => array('visible' => array(':input[name="renderer"]' => array('value' => 'quicktabs'))),
      '#weight' => -5,
    );

    $form['hide_empty_tabs'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide empty tabs'),
      '#default_value' => isset($qt->hide_empty_tabs) ? $qt->hide_empty_tabs : 0,
      '#description' => t('Empty and restricted tabs will not be displayed. Could be useful when the tab content is not accessible.<br />This option does not work in ajax mode.'),
      '#weight' => -4,
    );

    $form['qt_wrapper'] = array(
      '#tree' => FALSE,
      '#weight' => -3,
      '#prefix' => '<div class="clear-block" id="quicktabs-tabs-wrapper">',
      '#suffix' => '</div>',
    );

    $form['qt_wrapper']['tabs'] = array(
      '#tree' => TRUE,
      '#prefix' => '<div id="quicktab-tabs">',
      '#suffix' => '</div>',
      '#theme' => 'quicktabs_admin_form_tabs',
    );
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }
}
