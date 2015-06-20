<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Form\QuicktabEditForm.php
 */
namespace Drupal\quicktabs\Form;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\Core\Entity\EntityForm;

/**
 * Class QuicktabEditForm
 *
 */
class QuicktabEditForm extends EntityForm {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'quicktab edit';
  }

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $renderer_options = array('accordian', 'quicktabs', 'ui_tabs');
    $config = $this->config('quicktabs.settings');
    $form['title'] = array(
      '#title' => $this->t('Title'),
      '#description' => $this->t('This will appear as the block title.'),
      '#type' => 'textfield',
      // '#default_value' => isset($qt->title) ? $qt->title : '',
      '#weight' => -9,
      '#required' => TRUE,
      '#placeholder' => $this->t('Enter title'),
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#maxlength' => 32,
      '#machine_name' => array(
        //  'exists' => 'quicktabs_machine_name_exists',
        'source' => array('title'),
      ),
      '#description' => $this->t('A unique machine-readable name for this Quicktabs instance. It must only contain lowercase letters, numbers, and underscores. The machine name will be used internally by Quicktabs and will be used in the CSS ID of your Quicktabs block.'),
      '#weight' => -8,
    );

    $form['renderer'] = array(
      '#type' => 'select',
      '#title' => $this->t('Renderer'),
      '#options' => array(
        'accordian',
        'quicktabs',
        'ui_tabs'
      ),
      '#default_value' => $this->config('quicktabs.settings')->get('renderer'),
      '#description' => $this->t('Choose how to render the content.'),
      '#weight' => -7,
    );

    $form['ajax'] = array(
      '#type' => 'radios',
      '#title' => t('Ajax'),
      '#options' => array(
        TRUE => $this->t('Yes') . ': ' . t('Load only the first tab on page view'),
        FALSE => $this->t('No') . ': ' . t('Load all tabs on page view.'),
      ),
      '#default_value' => $config->get('ajax'),
      '#description' => $this->t('Choose how the content of tabs should be loaded.<p>By choosing "Yes", only the first tab will be loaded when the page first viewed. Content for other tabs will be loaded only when the user clicks the other tab. This will provide faster initial page loading, but subsequent tab clicks will be slower. This can place less load on a server.</p><p>By choosing "No", all tabs will be loaded when the page is first viewed. This will provide slower initial page loading, and more server load, but subsequent tab clicks will be faster for the user. Use with care if you have heavy views.</p><p>Warning: if you enable Ajax, any block you add to this quicktabs block will be accessible to anonymous users, even if you place role restrictions on the quicktabs block. Do not enable Ajax if the quicktabs block includes any blocks with potentially sensitive information.</p>'),
      //'#states' => array('visible' => array(':input[name="renderer"]' => array('value' => 'quicktabs'))),
      '#weight' => -6,
    );

    $form['style'] = array(
      '#type' => 'value',
      '#value' => 'nostyle',
    );


    $form['hide_empty_tabs'] = array(
      '#type' => 'checkbox',
      '#title' => $this->t('Hide empty tabs'),
      //    '#default_value' => isset($qt->hide_empty_tabs) ? $qt->hide_empty_tabs : 0,
      '#description' => $this->t('Empty and restricted tabs will not be displayed. Could be useful when the tab content is not accessible.<br />This option does not work in ajax mode.'),
      '#weight' => -4,
    );

    $form['qt_wrapper'] = array(
      '#tree' => FALSE,
      '#weight' => -3,
      '#prefix' => '<div class="clear-block" id="quicktabs-tabs-wrapper">',
      '#suffix' => '</div>',
    );

    $form['qt_wrapper']['tabs'] = array(
      '#type' => 'table',
      '#header' => array(
        t('Tab title'),
        t('Tab weight'),
        t('Tab type'),
        t('Tab content'),
        t('Operations'),
      ),
      '#tabledrag' => array(
        array(
          'action' => 'order',
          'relationship' => 'sibling',
          'group' => 'qt-tabs-weight',
        ),
      ),
      '#tree' => TRUE,
      '#prefix' => '<div id="quicktab-tabs">',
      '#suffix' => '</div>',
      //'#theme' => 'quicktabs_admin_form_tabs',
    );

    $form['qt_wrapper']['tabs']['operations'] = array(
      '#type' => 'operations',
      '#links' => array()
    );
    $form['qt_wrapper']['tabs']['operations']['#links']['edit'] = array(
      'title' => $this->t('Edit'),
      'url' => Url::fromRoute('entity.quicktabs.edit'),
    );
    $form['qt_wrapper']['tabs_more'] = array(
      '#type' => 'submit',
      '#prefix' => '<div id="add-more-tabs-button">',
      '#suffix' => '<label for="edit-tabs-more">' . t('Add tab') . '</label></div>',
      '#value' => t('More tabs'),
      '#attributes' => array(
        'class' => array('add-tab'),
        'title' => t('Click here to add more tabs.')
      ),
      '#weight' => 1,
      '#submit' => array('quicktabs_more_tabs_submit'),
      '#ajax' => array(
        'callback' => 'quicktabs_ajax_callback',
        'wrapper' => 'quicktab-tabs',
        'effect' => 'fade',
      ),
      '#limit_validation_errors' => array(),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function save(array $form, FormStateInterface $form_state) {
    $title = $form_state->getValue('title');
    $id = $form_state->getValue('id');
    $renderer = $form_state->getValue('renderer');
    $ajax = $form_state->getValue('ajax');
    $hide_empty_tabs = $form_state->getValue('hide_empty_tabs');
    $entity = $this->entity;
    $entity->set('title',$title);
    $entity->set('id',$id);
    $entity->set('renderer',$renderer);
    $entity->set('ajax',$ajax);
    $entity->set('hide_empty_tabs',$hide_empty_tabs);
    $status = $entity->save();
    if($status==SAVED_UPDATED)
      $form_state->setRedirect('quicktabs.add');
  }

}
