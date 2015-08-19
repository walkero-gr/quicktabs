<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Tests\QuicktabsAdminTest.php.
 */

namespace Drupal\quicktabs\Tests;

use Drupal\node\Tests\NodeTestBase;
use Drupal\Core\Language\Language;
use Drupal\Node\Entity\Node;
use Drupal\simpletest\WebTestBase;

/**
 * Add, edit and delete quicktabs.
 * @ingroup Quicktabs
 * @group Quicktabs
 */
class QuicktabsAdminTest extends WebTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('quicktabs', 'node');


  /**
   * A user to test with appropriate permissions.
   */
  private $admin_user;

  function setUp() {
    parent::setUp();

    $admin_user = $this->drupalCreateUser(array('access administration pages', 'administer quicktabs', 'administer nodes'));
    $this->drupalLogin($admin_user);

    for ($i = 0; $i < 5; $i++) {
      $node = array();
      $node['type'] = 'page';
      $node['uid'] = $i;
      $node['title'] = 'This is node number '. ($i+1);
      $node['body'][Language::LANGCODE_NOT_SPECIFIED][0]['value'] = $this->randomString(255);
      $loader = Node::create('node',$node);
      $loader->save();
    }
  }
}