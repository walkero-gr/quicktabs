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

    $this->$admin_user = $this->drupalCreateUser(array(
      'access administration pages',
      'administer quicktabs',
      'administer nodes'
    ));
    $this->drupalLogin($admin_user);

    for ($i = 0; $i < 5; $i++) {
      $node = array();
      $node['type'] = 'page';
      $node['uid'] = $i;
      $node['title'] = 'This is node number ' . ($i + 1);
      $node['body'][Language::LANGCODE_NOT_SPECIFIED][0]['value'] = $this->randomString(255);
      $loader = Node::create('node', $node);
      $loader->save();
    }
  }

  /**
   * Create a Quicktabs instance through the UI and ensure that it is saved properly.
   */
  function testQuicktabsAdmin() {

    $edit = EntityInterface::create(array(
      'machine_name' => strtolower($this->randomMachineName()),
      'title' => $this->randomMachineName(),
      'ajax' => 0,
      'hide_empty_tabs' => FALSE,
      'renderer' => 'quicktabs',
    ));

    $edit->save();


    // Add a new Quicktabs instance using the UI.
    $saved = $edit;
    // We'll be using the $saved array to compare against the Quicktabs instance
    // that gets created. However, hierarchical form elements are dealt with
    // differenly so we can't include them in the $saved array like this.
    $tab_title_first = $this->randomMachineName();
    $tab_title_second = $this->randomMachineName();
    $edit += array(
      'tabs[0][type]' => 'node',
      'tabs[0][node][nid]' => 1,
      'tabs[0][node][view_mode]' => 'full',
      'tabs[0][title]' => $tab_title_first,
      'tabs[0][weight]' => 0,
      'tabs[1][type]' => 'node',
      'tabs[1][node][nid]' => 2,
      'tabs[1][node][view_mode]' => 'full',
      'tabs[1][title]' => $tab_title_second,
      'tabs[1][weight]' => 1,
    );
    // Now add on the tabs info to the $saved array - it's the same as what we
    // put in the edit form but we need it in proper array format.
    $saved['tabs'] = array(
      0 => array(
        'type' => 'node',
        'nid' => 1,
        'view_mode' => 'full',
        'title' => $tab_title_first,
        'weight' => 0
      ),
      1 => array(
        'type' => 'node',
        'nid' => 2,
        'view_mode' => 'full',
        'title' => $tab_title_second,
        'weight' => 1
      )
    );
    $this->drupalPost('admin/structure/quicktabs/add', $edit, t('Save'));

    // Check that the quicktabs object is in the database.
    $quicktabs = quicktabs_load($edit['machine_name']);
    $this->assertTrue($quicktabs != FALSE, t('Quicktabs instance found in database'));

    // Check each individual property of the quicktabs and make sure it was set.
    foreach ($saved as $property => $value) {
      if ($property == 'tabs') {
        // Add some extra default values that we didn't include on the form, for
        // the sake of comparing the two tabs arrays.
        foreach ($value as &$val) {
          $val += array('hide_title' => 1);
        }
      }
      $this->assertEqual($quicktabs->$property, $value, t('Quicktabs property %property properly saved.', array('%property' => $property)));
    }

    // Edit the Quicktabs instance through the UI.
    $edit = array(
      'title' => $this->randomMachineName(),
      'ajax' => 1,
      'hide_empty_tabs' => TRUE,
      'renderer' => 'ui_tabs',
      'default_tab' => 1,
    );

    $saved = $edit;
    $tab_title_first = $this->randomMachineName();
    $tab_title_second = $this->randomMachineName();
    $edit += array(
      'tabs[0][title]' => $tab_title_first,
      'tabs[0][weight]' => 1,
      'tabs[0][node][nid]' => 3,
      'tabs[0][node][view_mode]' => 'teaser',
      'tabs[0][node][hide_title]' => FALSE,
      'tabs[1][title]' => $tab_title_second,
      'tabs[1][weight]' => 0,
      'tabs[1][node][nid]' => 4,
      'tabs[1][node][view_mode]' => 'teaser',
      'tabs[1][node][hide_title]' => 1,
    );
    $saved['tabs'] = array(
      0 => array(
        'type' => 'node',
        'nid' => 4,
        'title' => $tab_title_second,
        'weight' => 0,
        'view_mode' => 'teaser',
        'hide_title' => 1
      ),
      1 => array(
        'type' => 'node',
        'nid' => 3,
        'title' => $tab_title_first,
        'weight' => 1,
        'view_mode' => 'teaser',
        'hide_title' => 0
      )
    );
    $this->drupalPost('admin/structure/quicktabs/manage/' . $quicktabs->machine_name . '/edit', $edit, t('Save'));

    // Reset static vars because ctools will have cached the original $quicktabs object
    drupal_static_reset();
    // Check that the quicktabs object is in the database.
    $edited_qt = quicktabs_load($quicktabs->machine_name);
    $this->assertTrue($edited_qt != FALSE, t('Quicktabs instance found in database'));

    // Check each individual property of the quicktabs and make sure it was set.
    foreach ($saved as $property => $value) {
      $this->assertEqual($edited_qt->$property, $value, t('Quicktabs property %property properly saved.', array('%property' => $property)));
    }

    // Delete the Quicktabs instance through the UI.
    $this->drupalPost('admin/structure/quicktabs/manage/' . $quicktabs->machine_name . '/delete', array(), t('Delete'));
    // Reset static vars because ctools will have cached the original $quicktabs object
    drupal_static_reset();
    // Check that the quicktabs object is no longer in the database.
    $this->assertNull(quicktabs_load($quicktabs->machine_name), t('Quicktabs instance not found in database'));
  }
}

