*******************************************************************************
$Id$

quicktabs

Description:
-------------------------------------------------------------------------------

  This module provides a form for admins to create a block of tabbed content by
selecting first the desired number of tabs and then selecting either an existing
view or an existing block as the content of each tab. When submitted the form
generates code that needs to be pasted into a new block that is php-enabled.
In the future this step won't be necessary as the module will create the block 
automatically when the form is submitted.



Installation & Use:
-------------------------------------------------------------------------------

1.  Enable module in module list located at administer > build > modules.
2.  Go to admin/build/quicktabs.
3.  Enter number of tabs required and click 'Next'
4.  For each tab, enter a title. Select either 'block' or 'view' from the Type drop
    down and then select the required block/view.
5.  Click 'Next'
6.  Copy the resultant code and then go to admin/build/block and click on "Add
    Block" and paste in the code as the body of the block. Give the block a title
    and ensure that the input format is set to 'php code'.
7.  Save the block and configure & enable it as required.




Author:
-------------------------------------------------------------------------------

Katherine Bailey <katherine@raincitystudios.com>
http://drupal.org/user/172987


TODO
-------------------------------------------------------------------------------
+ add a validation function to the form
+ change the functionality of the form to automatically create the block so that
  the  user doesn't have to copy and paste any code
+ exclude this module's blocks from the block selection dropdown




