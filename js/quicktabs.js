Drupal.behaviors.quicktabs = function (context) {
  $('.quicktabs_wrapper:not(.quicktabs-processed)', context).addClass('quicktabs-processed').each(function () {
    var i = 0;
    $(this).find('div.quicktabs:first-child').show()
    .end()
    .find('ul.quicktabs_tabs li:first-child').addClass('active')
    .end()
    .find('ul.quicktabs_tabs li a').each(function(){
      this.myTabIndex = i++;
      $(this).bind('click', quicktabsClick);
    });
  });
};

var quicktabsClick = function() {
  var tabIndex = this.myTabIndex;
  $(this).parents('li').siblings().removeClass('active');
  $(this).parents('li').addClass('active');
  if ( $(this).hasClass('qt_ajax_tabs') ) {
    var viewDetails = this.id.split('-');
    var $container = $('div#quicktabs_ajax_container_' + viewDetails[1]);
    if (viewDetails[0] == 'node') {
      $.get(Drupal.settings.basePath + 'quicktabs/ajax/node/' + viewDetails[3], null, function(data){
        var result = Drupal.parseJson(data);
        $container.html(result['data']);
      });
    } else {
      var args;
      if (viewDetails.length == 6) {
        args = '/' + viewDetails[5];
      } else {
        args = '';
      }
      $.get(Drupal.settings.basePath + 'quicktabs/ajax/views/' + viewDetails[3] + '/' + viewDetails[4] + args, null, function(data){
        var result = Drupal.parseJson(data);
        $container.html(result['data']);
      });
    }
  } else {
    $(this).parents('.quicktabs_wrapper').find('div.quicktabs').hide();
    $(this).parents('.quicktabs_wrapper').find('div.quicktabs:eq('+tabIndex+')').show();
  }
  return false;
}