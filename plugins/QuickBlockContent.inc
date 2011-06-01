<?php

/**
 * Class for tab content of type "block" - this is for rendering a block as tab
 * content.
 */
class QuickBlockContent extends QuickContent {

  public function options_form($delta) {
    $tab = $this->item;
    $form = array();
    $form['block']['bid'] = array(
      '#type' => 'select',
      '#options' => quicktabs_get_blocks(),
      '#default_value' => isset($tab['bid']) ? $tab['bid'] : '',
      '#title' => t('Select a block'),
    );
    $form['block']['hide_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide the title of this block'),
      '#default_value' => isset($tab['hide_title']) ? $tab['hide_title'] : 1,
    );
    return $form;
  }

  public function render_item($hide_empty = FALSE) {
    $item = $this->item;
    $output = array();
    if (isset($item['bid'])) {
      if (module_exists('block')) {
        $pos = strpos($item['bid'], '_delta_');
        $module = drupal_substr($item['bid'], 0, $pos);
        $delta = drupal_substr($item['bid'], $pos + 7);
  
        $block = block_load($module, $delta);
        $block->region = 'quicktabs_tabpage';
  
        if ($block_arr = _block_render_blocks(array($block))) {
          if ($item['hide_title']) {
            $block_arr["{$block->module}_{$block->delta}"]->subject = FALSE;
          }
          if (!empty($block_arr["{$block->module}_{$block->delta}"]->content)) {
            $build = _block_get_renderable_array($block_arr);
            $output = $build;
          }
        }
      }
      elseif (!$hide_empty) {
        $output['#markup'] = t('Block module is not enabled, cannot display content.');
      }
    }
    return $output;
  }
  
  public function ajax_render_item($args) {
    $item = array();
    list($item['bid'], $item['hide_title']) = $args;
    $this->item = array_merge($this->item, $item);
    $output = $this->render_item();
    return !empty($output) ? drupal_render($output) : '';
  }

  public function get_ajax_path($name, $index) {
    $path = parent::get_ajax_path($name, $index);
    $path .= '/'. $this->item['bid'] . '/'. $this->item['hide_title'];
    return $path;
  }
}