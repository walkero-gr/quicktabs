<?php

/**
 * @file
 * Contains \Drupal\quicktabs\QuicktabsFormController.
 */

namespace Drupal\quicktabs;

use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\EntityFormController;

/**
 * Form controller for the shortcut set entity edit forms.
 */
class QuicktabsFormController extends EntityFormController {

  /**
   * Overrides \Drupal\Core\Entity\EntityFormController::form().
   */
  public function form(array $form, array &$form_state, EntityInterface $quicktabs) {
    $form = parent::form($form, $form_state, $quicktabs);

    //$form['label'] = array(
    //  '#type' => 'textfield',
    //  '#title' => t('Quicktabs title'),
    //  '#description' => t('The new Quicktabs entity.'),
    //  '#required' => TRUE,
    //  '#default_value' => $entity->label(),
    //);
    //$form['id'] = array(
    //  '#type' => 'machine_name',
    //  '#machine_name' => array(
    //    'exists' => 'shortcut_set_load',
    //    'source' => array('label'),
    //    'replace_pattern' => '[^a-z0-9-]+',
    //    'replace' => '-',
    //  ),
    //  '#default_value' => $entity->id(),
    //  '#disabled' => !$entity->isNew(),
    //  // This id could be used for menu name.
    //  '#maxlength' => 23,
    //);

    $form = $this->mainForm($form_state, $quicktabs);

    // If creating a new Quicktabs instance, start off with 2 empty tabs.
    if (empty($quicktabs->tabs)) {
      $quicktabs->tabs = array(
        0 => array(),
        1 => array(),
      );
    }

    // If the "Add another" button was clicked, we need to increment the number of
    // tabs by one.
    if (isset($form_state['num_tabs']) && $form_state['num_tabs'] > count($quicktabs->tabs)) {
      $quicktabs->tabs[] = array();
    }
    $form_state['num_tabs'] = count($quicktabs->tabs);

    // If the "Remove" button was clicked for a tab, we need to remove that tab
    // from the form.
    if (isset($form_state['to_remove'])) {
      unset($quicktabs->tabs[$form_state['to_remove']]);
      unset($form_state['to_remove']);
      $form_state['num_tabs']--;
    }

    $tab_titles = array();
    // Add current tabs to the form.
    foreach ($quicktabs->tabs as $delta => $tab) {
      $tab['delta'] = $delta;
      $form['qt_wrapper']['tabs'][$delta] = $this->tabForm($tab, $quicktabs);
      if (isset($tab['title'])) {
        $tab_titles[$delta] = $tab['title'];
      }
    }
    // If there's only one tab, it shouldn't be removeable.
    if (count($quicktabs->tabs) == 1) $form['qt_wrapper']['tabs'][$delta]['remove']['#access'] = FALSE;

    $form['default_tab'] = array(
      '#type' => 'select',
      '#title' => t('Default tab'),
      '#options' => $tab_titles,
      '#default_value' => isset($quicktabs->default_tab) ? $quicktabs->default_tab : 0,
      '#access' => !empty($tab_titles),
      '#weight' => -5,
    );

    return $form;
  }

  protected function mainForm($form_state, $quicktabs) {

    // The contents of $entity will either come from the db or from $form_state.
    if (isset($form_state['values']['title'])) {
      $quicktabs = $this->getEntity($form_state);
    }

    $form['label'] = array(
      '#title' => t('Title'),
      '#description' => t('This will appear as the block title.'),
      '#type' => 'textfield',
      '#default_value' => isset($quicktabs->label) ? $quicktabs->label : '',
      '#weight' => -9,
      '#required' => TRUE,
    );

    $form['id'] = array(
      '#type' => 'machine_name',
      '#maxlength' => 32,
      '#machine_name' => array(
        'exists' => 'quicktabs_machine_name_exists',
        'source' => array('title'),
      ),
      '#description' => t('A unique machine-readable name for this Quicktabs instance. It must only contain lowercase letters, numbers, and underscores. The machine name will be used internally by Quicktabs and will be used in the CSS ID of your Quicktabs block.'),
      '#weight' => -8,
    );

    if (!empty($quicktabs->id)) {
      $form['id']['#default_value'] = $quicktabs->id;
      $form['id']['#disabled'] = TRUE;
      $form['id']['#value'] = $quicktabs->id;
    }

    //ctools_include('plugins');
    //$renderers = ctools_get_plugins('quicktabs', 'renderers');
    //$renderer_options = array();
    //foreach ($renderers as $name => $info) {
    //  // Add the renderer to the dropdown list of renderers
    //  $renderer_options[$name] = $name;
    //}
    //ksort($renderer_options);

    $selected_renderer = isset($quicktabs->renderer) ? $quicktabs->renderer : 'quicktabs';
    $form['renderer'] = array(
      '#type' => 'select',
      '#title' => t('Renderer'),
      '#options' => array('quicktabs' => 'quicktabs'),
      '#default_value' => $selected_renderer,
      '#description' => t('Choose how to render the content.'),
      '#weight' => -7,
      '#ajax' => array(
        'event' => 'change',
        'callback' => 'quicktabs_renderer_dropdown_callback',
        'wrapper' => 'options-replace',
      ),
    );

    // Get the form element definitions for renderer features.
    //$features = quicktabs_get_renderer_features();
    $supported_features = array();

    // Add the renderer options form elements to the form, to be shown only if the
    // renderer in question is selected. Unfortunately we can't use states for this
    // because of http://drupal.org/node/735528, i.e. in some cases we want to show
    // a particular element if the value of another element is 'foo' OR 'bar'.
    $form['options'] = array(
      '#tree' => TRUE,
      '#weight' => -6,
      '#prefix' => '<div id="options-replace">',
      '#suffix' => '</div>',
    );

    //if ($class = ctools_plugin_load_class('quicktabs', 'renderers', $selected_renderer, 'handler')) {
    //  // Keep track of which features (e.g. ajax, history) this renderer supports
    //  foreach (array_keys($features) as $feature_name) {
    //    $supported_features[$feature_name] = call_user_func_array(array($class, 'supportsFeature'), array($feature_name));
    //  }
    //  // Get the renderer-specific options form elements
    //  $renderer_form_options = call_user_func_array(array($class, 'optionsForm'), array($qt));
    //}

    // Add the form elements for supported features.
    //foreach (array_filter($supported_features) as $feature_name => $supported) {
    //  $form_element = $features[$feature_name]['form_element'];
    //  $form_element['#default_value'] = isset($qt->options[$feature_name]) ? $qt->options[$feature_name] : $form_element['#default_value'];
    //  $form['options'][$feature_name] = $form_element;
    //}
    //
    //// Add renderer-specific options.
    //$form['options'] += $renderer_form_options;

    $form['hide_empty_tabs'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide empty tabs'),
      '#default_value' => isset($quicktabs->hide_empty_tabs) ? $quicktabs->hide_empty_tabs : 0,
      '#description' => t('Empty and restricted tabs will not be displayed. Could be useful when the tab content is not accessible.<br />This option does not work in ajax mode.'),
      '#weight' => -4,
    );

    // Add a wrapper for the tabs and Add Another Tab button.
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

    $form['qt_wrapper']['tabs_more'] = array(
      '#type' => 'submit',
      '#prefix' => '<div id="add-more-tabs-button">',
      '#suffix' => '<label for="edit-tabs-more">' . t('Add tab') . '</label></div>',
      '#value' => t('More tabs'),
      '#attributes' => array('class' => array('add-tab'), 'title' => t('Click here to add more tabs.')),
      '#weight' => 1,
      '#submit' => array('quicktabs_more_tabs_submit'),
      '#ajax' => array(
        'callback' => 'quicktabs_ajax_callback',
        'wrapper' => 'quicktab-tabs',
        'effect' => 'fade',
      ),
      '#limit_validation_errors' => array(),
    );

    $form['actions']['submit']['#value'] = t('Save');
    return $form;
  }

  protected function tabForm($tab, $quicktabs) {
    $form['#tree'] = TRUE;
    $delta = $tab['delta'];

    $form['weight'] = array(
      '#type' => 'weight',
      '#default_value' => isset($tab['weight']) ? $tab['weight'] : $delta-100,
      '#delta' => 100,
    );

    $form['title'] = array(
      '#type' => 'textfield',
      '#size' => '10',
      '#default_value' => isset($tab['title']) ? $tab['title'] : '',
    );

    // Load all "contents" plugins to display a choice of content types.
    //ctools_include('plugins');
    //$contents = ctools_get_plugins('quicktabs', 'contents');
    //foreach ($contents as $name => $info) {
    //  if (isset($info['dependencies'])) {
    //    foreach ($info['dependencies'] as $dep) {
    //      // Do not load the options form for any plugin that is missing dependencies.
    //      if (!module_exists($dep)) continue 2;
    //    }
    //  }
    //  $tabtypes[$name] = $name;
    //  $content_provider = quick_content_factory($name, $tab);
    //  $form = array_merge_recursive($form, $content_provider->optionsForm($delta, $qt));
    //}
    $tabtypes = array('block' => 'block');
    $form['type'] = array(
      '#type' => 'radios',
      '#options' => $tabtypes,
      '#default_value' => isset($tab['type']) ? $tab['type'] : key($tabtypes),
    );

    $form['remove'] = array(
      '#type' => 'submit',
      '#prefix' => '<div>',
      '#suffix' => '<label for="edit-remove">' . t('Delete') . '</label></div>',
      '#value' => 'remove_' . $delta,
      '#attributes' => array('class' => array('delete-tab'), 'title' => t('Click here to delete this tab.')),
      '#submit' => array('quicktabs_remove_tab_submit'),
      '#ajax' => array(
        'callback' => 'quicktabs_ajax_callback',
        'wrapper' => 'quicktab-tabs',
        'method' => 'replace',
        'effect' => 'fade',
      ),
      '#limit_validation_errors' => array(),
    );

    return $form;
  }

  /**
   * Overrides \Drupal\Core\Entity\EntityFormController::validate().
   */
  public function validate(array $form, array &$form_state) {
    parent::validate($form, $form_state);
    $quicktabs = $this->getEntity($form_state);

    if (!isset($form_state['values']['tabs'])) {
      form_set_error('', t('At least one tab should be created.'));
    }
    else {
      foreach ($form_state['values']['tabs'] as $j => $tab) {
        if (empty($tab['title'])) {
          form_set_error('tabs][' . $j . '][title', t('Title is required for each tab.'));
        }
      }
    }
  }

  /**
   * Overrides \Drupal\Core\Entity\EntityFormController::save().
   */
  public function save(array $form, array &$form_state) {
    //$entity = $this->getEntity($form_state);

    $quicktabs = $this->convertFormToQuicktabs($form_state);
    $is_new = !$quicktabs->getOriginalID();
    $quicktabs->save();

    if ($is_new) {
      drupal_set_message(t('The %set_name Quicktabs instance has been created. You can edit it from this page.', array('%set_name' => $quicktabs->label())));
    }
    else {
      drupal_set_message(t('Updated Quicktabs instance to %set-name.', array('%set-name' => $quicktabs->label())));
    }

    $form_state['redirect'] = 'admin/structure/quicktabs/manage/' . $quicktabs->id();
  }

  /**
   * Overrides \Drupal\Core\Entity\EntityFormController::delete().
   */
  public function delete(array $form, array &$form_state) {
    $quicktabs = $this->getEntity($form_state);
    $form_state['redirect'] = 'admin/config/user-interface/shortcut/manage/' . $quicktabs->id() . '/delete';
  }

  /**
   * Helper function to convert the data on admin form into a QT entity.
   */
  function convertFormToQuicktabs($form_state) {
    $formvalues_tabs = array();
    if (!empty($form_state['values']['tabs'])) {
      foreach ($form_state['values']['tabs'] as $j => $tab) {
        $formvalues_tabs[$j] = $tab[$tab['type']];
        $formvalues_tabs[$j]['title'] = $tab['title'];
        $formvalues_tabs[$j]['weight'] = $tab['weight'];
        $formvalues_tabs[$j]['type'] = $tab['type'];
        $weight[$j] = $tab['weight'];
      }
      array_multisort($weight, SORT_ASC, $formvalues_tabs);
    }
    $renderer = $form_state['values']['renderer'];
    $qt = $form_state['entity'];
    $qt->title = $form_state['values']['title'];
    $qt->default_tab = isset($form_state['values']['default_tab']) ? $form_state['values']['default_tab'] : 0;
    $qt->hide_empty_tabs = $form_state['values']['hide_empty_tabs'];
    $qt->renderer = $renderer;
    $qt->tabs = $formvalues_tabs;
    $qt->options = isset($form_state['values']['options']) ? $form_state['values']['options'] : array();

    if (isset($form_state['values']['machine_name'])) {
      $qt->machine_name = $form_state['values']['machine_name'];
    }

    return $qt;
  }

}
