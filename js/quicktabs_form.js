$(document).ready(function(){
  $('#quicktabs-form div.form-item :input[@name^="bid"]').parent().hide();
  var showhide = function() {
    if (this.value == 'block') {
      $(this).parent().parent()
      .find('div.form-item :input[@name^="vid"]').parent().hide();
      $(this).parent().parent()
      .find('div.form-item :input[@name^="args"]').parent().hide();
      $(this).parent().parent()
      .find('div.form-item :input[@name^="limit"]').parent().hide();
      $(this).parent().parent()
      .find('div.form-item :input[@name^="bid"]').parent().show();    
   } else {
      $(this).parent().parent()
      .find('div.form-item :input[@name^="bid"]').parent().hide();      
      $(this).parent().parent()
      .find('div.form-item :input[@name^="vid"]').parent().show();
      $(this).parent().parent()
      .find('div.form-item :input[@name^="args"]').parent().show();
      $(this).parent().parent()
      .find('div.form-item :input[@name^="limit"]').parent().show();  
   }
  };
  $('#quicktabs-form div.form-item :input[@name^="type"]').bind('change', showhide);
});