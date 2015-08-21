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
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Entity\Entity;
use Drupal\Core\Database\Database;

  /**
 * Add, edit and delete quicktabs.
 *
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
   *
   */
  private $admin_user;

  function setUp() {
    parent::setUp();

    $this->admin_user = $this->drupalCreateUser(array(
      'administer quicktabs',
      'administer nodes'
    ));
    $this->drupalLogin($this->admin_user);

    for ($i = 0; $i < 5; $i++) {
      $node = array();
      $node['type'] = 'page';
      $node['uid'] = $i;
      $node['title'] = 'This is node number ' . ($i + 1);
      $node['body'][Language::LANGCODE_NOT_SPECIFIED][0]['value'] = $this->randomString(255);
      $loader = Node::create($node);
      $loader->save();
    }
  }

  /**
   * Create a Quicktabs instance through the UI and ensure that it is saved properly.
   */
  function testQuicktabsAdmin() {
    $this->drupalLogin($this->admin_user);
    $edit = \Drupal::entityManager()->getStorage('settings')->create(array(
      'id' => strtolower($this->randomMachineName()),
      'title' => $this->randomMachineName(),
      'renderer' => 1,
      'ajax' => FALSE,
      'style' => 'nostyle',
      'hide_empty_tabs' => 1,
    ));

    $edit->save();

  }
}

