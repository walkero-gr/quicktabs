(function ($) {

Drupal.behaviors.qt_accordion = {
  attach: function (context, settings) {
    options = Drupal.settings.quicktabs;
    $('.quick-accordion').once(function(){
      var qtKey = 'qt_' + this.id.substring(this.id.indexOf('-') +1);
      var options = Drupal.settings.quicktabs[qtKey].options;
      var active_tab = parseInt(Drupal.settings.quicktabs[qtKey].active_tab);
      options.active = active_tab;
      options.event = 'change';
      $(this).accordion(options).addClass("init");
    });

    var accordions = $(".quick-accordion");
    acc_selector = "h3 a";

    // Add href value to links.
    $(".accordion a").each(function(index, value) {
      id = $(this).closest( '.quick-accordion' ).attr( 'id' );
      $(this).attr("href", "#" + id + "_" + index);
    });

    accordions.find( acc_selector ).click(function(event){
      // Keep the link from firing.
      event.preventDefault();
      var state = {},

        id = $(this).closest( '.quick-accordion' ).attr( 'id' ),

        idx = $(this).parent().prevAll('h3').length;

      // Get id for this widget.
      el = id.split("quickset-");
      key = 'qt_' + el[1];
      // Only fire if history is set.
      if (options[key].history) {
        state[ id ] = idx;
        $.bbq.pushState( state );
      }
      else {
        $('#' + id).accordion('activate', idx);
      }
    });

    $(window).bind( 'hashchange', function(e) {
      params = $.deparam.fragment();
      // Should only fire once when page loads.
      if (accordions.hasClass("init")) {
        for (var key in options) {
          name = 'quickset-' + options[key].name;
          active_tab = parseInt(options[key].active_tab);
          // If there is an active tab and the tab isn't already defined by a hash.
          if (active_tab > 0 && !params[name]) {
            $("#" + name).accordion('activate', active_tab);
          }
        }
        accordions.removeClass("init");
      }

      accordions.each(function(){
        var idx = e.getState( this.id, true );
        // Only activate tab if it has a hash.
        if (params[$(this).attr('id')]) {
          $(this).accordion('activate', idx);
        }
      });
    });

    $(window).trigger( 'hashchange' );

  }
}

})(jQuery);
