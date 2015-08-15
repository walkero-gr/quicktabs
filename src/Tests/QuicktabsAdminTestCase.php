<?php
/**
 * @file
 * Contains \Drupal\quicktabs\Tests\QuicktabsAdminTestCase.php.
 */

namespace Drupal\quicktabs\Tests;

use Drupal\simpletest\WebTestBase;

/**
 * Add, edit and delete quicktabs.
 * @ingroup Quicktabs
 * @group Quicktabs
 */
class QuicktabsAdminTestCase extends WebTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('quicktabs');
}