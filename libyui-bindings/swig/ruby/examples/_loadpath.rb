DIR = File.dirname(__FILE__)

$: << DIR

if ENV["LIBYUI_GEM"] # set LIBYUI_GEM to test gem-based libyui-bindings
  require 'rubygems'
else
  # Autotools binary
  $:.unshift File.expand_path(File.join(DIR,"../.libs"))

  # cmake binary
  $:.unshift File.expand_path(File.join(DIR,"../../../build/swig/ruby"))

  # cmake local for openwsman.rb
  $:.unshift File.expand_path(File.join(DIR,".."))
end
