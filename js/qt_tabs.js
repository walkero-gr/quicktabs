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
        select: function(event, ui) {
          $.data(ui.tab, "load.tabs", "");
        }
      }
      $(this).tabs(tab_options);
    });
  }

})
})(jQuery)