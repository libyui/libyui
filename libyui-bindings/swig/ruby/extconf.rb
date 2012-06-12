#
# extconf.rb for yui Gem
#
require 'rbconfig'
Config::MAKEFILE_CONFIG['CC'] = 'g++'
require 'mkmf'
# $CFLAGS = "#{$CFLAGS} -Werror"

have_library('stdc++')

unless have_library('libyui', 'YUI::ui')
  STDERR.puts "Cannot find libyui"
  STDERR.puts "Is libyui-devel installed ?"
end
find_header 'YUI.h', '/usr/include/yui'

$CPPFLAGS = "-I/usr/include/yui -I.."

create_makefile('_yui')
