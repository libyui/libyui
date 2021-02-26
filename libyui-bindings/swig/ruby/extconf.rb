#
# extconf.rb for yui Gem
#

require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

RbConfig::CONFIG['CC'] = "g++"
RbConfig::CONFIG['CPP'] = "g++ -E"

have_library('stdc++')

unless have_library('yui', 'YUI::widgetFactory', 'yui/YUI.h')
  STDERR.puts "Cannot find libyui"
  STDERR.puts "Is libyui-devel installed ?"
end
find_header 'YUI.h', '/usr/include/yui'

create_makefile('_yui')
