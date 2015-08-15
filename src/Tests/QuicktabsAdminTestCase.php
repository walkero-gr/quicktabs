<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Tests\QuicktabsAdminTestCase.php.
 */

namespace Drupal\quicktabs\Tests;

use Drupal\node\Tests\NodeTestBase;
use Drupal\Core\Language\Language;

/**
 * Add, edit and delete quicktabs.
 * @ingroup Quicktabs
 * @group Quicktabs
 */
class QuicktabsAdminTestCase extends NodeTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('quicktabs');

  function setUp() {
    parent::setUp();

    $admin_user = $this->drupalCreateUser(array('access administration pages', 'administer quicktabs', 'administer nodes'));
    $this->drupalLogin($admin_user);

    for ($i = 0; $i < 5; $i++) {
      $node = new \stdClass();
      $node->type = 'page';
      $node->title = 'This is node number '. ($i+1);
      $node->body[Language::LANGCODE_NOT_SPECIFIED][0]['value'] = $this->randomString(255);
      node_object_prepare($node);
      node_save($node);
    }
  }


}