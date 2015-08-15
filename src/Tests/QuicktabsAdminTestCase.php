<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Tests\QuicktabsAdminTestCase.php.
 */

namespace Drupal\quicktabs\Tests;

use Drupal\node\Tests\NodeTestBase;

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
  }


}