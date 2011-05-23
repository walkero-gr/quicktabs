(function ($) {
Drupal.settings.views = Drupal.settings.views || {'ajax_path': '/views/ajax'};

Drupal.quicktabs = Drupal.quicktabs || {};

Drupal.quicktabs.getQTName = function (el) {
  return el.id.substring(el.id.indexOf('-') +1);
}

Drupal.behaviors.quicktabs = {
  attach: function (context, settings) {
    $.extend(true, Drupal.settings, settings);
    $('.quicktabs-wrapper:not(.quicktabs-processed)', context).addClass('quicktabs-processed').each(function(){
      Drupal.quicktabs.prepare(this);
    });
    if ($.fn.accordion) {
      $('.quick-accordion').each(function(){
        var qtKey = 'qt_' + Drupal.quicktabs.getQTName(this);
        var active_tab = parseInt(Drupal.settings.quicktabs[qtKey].active_tab);
        $(this).accordion({active: active_tab});
      });
    }
  }
}



// setting up the inital behaviours
Drupal.quicktabs.prepare = function(el) {
  // el.id format: "quicktabs-$name"
  var name = Drupal.quicktabs.getQTName(el);
  var $ul = $(el).find('ul.quicktabs-tabs:first');
  $ul.find('li a').each(function(){this.name = name}).each(Drupal.quicktabs.initialiseLink);
}

Drupal.quicktabs.initialiseLink = function(index, element) {
  if (!element.myTabIndex) {
    element.myTabIndex = index;
  }
  var tab = new Drupal.quicktabs.tab(element);
  var parent_li = $(element).parents('li').get(0);
  if ($(parent_li).hasClass('active')) {
    $(element).addClass('quicktabs-loaded');
  }

  $(element).once(function() {$(this).bind('click', {tab: tab}, Drupal.quicktabs.clickHandler);});
}

Drupal.quicktabs.clickHandler = function(event) {
  var tab = event.data.tab;
  var element = this;
  // Set clicked tab to active.
  $(this).parents('li').siblings().removeClass('active');
  $(this).parents('li').addClass('active');

  // Hide all tabpages.
  tab.container.children().addClass('quicktabs-hide');
  
  if (!tab.tabpage.hasClass("quicktabs-tabpage")) {
    tab = new Drupal.quicktabs.tab(element);
  }

  tab.tabpage.removeClass('quicktabs-hide');
  return false;
}

// constructor for an individual tab
Drupal.quicktabs.tab = function (el) {
  this.element = el;
  this.tabIndex = el.myTabIndex;
  this.name = el.name;
  var qtKey = 'qt_' + this.name;
  var i = 0;
  for (var key in Drupal.settings.quicktabs[qtKey].tabs) {
    if (i == this.tabIndex) {
      this.tabObj = Drupal.settings.quicktabs[qtKey].tabs[key];
      this.tabKey = key;
    }
    i++;
  }
  this.tabpage_id = 'quicktabs-tabpage-' + this.name + '-' + this.tabKey;
  this.container = $('#quicktabs-container-' + this.name);
  this.tabpage = this.container.find('#' + this.tabpage_id);
}

if (Drupal.ajax) {
  /**
   * Handle an event that triggers an AJAX response.
   *
   * When an event that triggers an AJAX response happens, this method will
   * perform the actual AJAX call. It is bound to the event using
   * bind() in the constructor, and it uses the options specified on the
   * ajax object.
   */
  Drupal.ajax.prototype.eventResponse = function (element, event) {
    // Create a synonym for this to reduce code confusion.
    var ajax = this;
  
    // Do not perform another ajax command if one is already in progress.
    if (ajax.ajaxing) {
      return false;
    }
  
    try {
      if (ajax.form) {
        // If setClick is set, we must set this to ensure that the button's
        // value is passed.
        if (ajax.setClick) {
          // Mark the clicked button. 'form.clk' is a special variable for
          // ajaxSubmit that tells the system which element got clicked to
          // trigger the submit. Without it there would be no 'op' or
          // equivalent.
          element.form.clk = element;
        }
  
        ajax.form.ajaxSubmit(ajax.options);
      }
      else {
        if (!$(element).hasClass('quicktabs-loaded')) {
          ajax.beforeSerialize(ajax.element, ajax.options);
          $.ajax(ajax.options);
          if ($(element).hasClass('qt-ajax-tab')) {
            $(element).addClass('quicktabs-loaded');
          }
        }
      }
    }
    catch (e) {
      // Unset the ajax.ajaxing flag here because it won't be unset during
      // the complete response.
      ajax.ajaxing = false;
      alert("An error occurred while attempting to process " + ajax.options.url + ": " + e.message);
    }
    return false;
  };
}


})(jQuery);
