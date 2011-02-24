(function($){
$(function() {
  $.fn.quicktabs = function(options) {
    options = $.extend(
      // Default options
      {
        qtWrapper: 'quicktabs'
      },

      // Option overrides, passed to the plugin
      options || {}
    );

    return this.each(function () {
      var tab_options = {
        idPrefix: "qt-" + options.qtWrapper + "-ui-tabs",
        cache: true,
        // We need to prevent the jQuery UI Tabs ajax behavior because it
        // basically steps on the toes of the Ajax Framework. By setting the
        // "load.tabs" property here to an empty string, we ensure that it won't
        // attempt to fetch the content at the URL of the href of our tab links.
        select: function(event, ui) {
          $.data(ui.tab, "load.tabs", "");
        }
      }
      $(this).tabs(tab_options);
    });
  }

})
})(jQuery)