$(document).ready(function(){
  $('div.quicktabs').hide();
  $('div.quicktabs:first-child').show();
  $('ul.quicktabs_tabs li:first-child').addClass('active');
  
  var clickFunction = function() {      
    var tabIndex = this.myTabIndex;
    $(this).parent().parent().parent().find('div.quicktabs').hide();
    $(this).parent().siblings().removeClass('active');
    $(this).parent().addClass('active');
    $(this).parent().parent().parent().find('div.quicktabs:eq('+tabIndex+')').show();
    return false;
  };
  $('ul.quicktabs_tabs').each(function(){
    var i = 0;
    $(this).find('li a').each(function() {
      this.myTabIndex = i++;
    });
  });
  $('ul.quicktabs_tabs li a').bind('click', clickFunction);
});