#
# Test loading of the bindings
#

require File.join(File.dirname(__FILE__),'_loadpath')

# test loading of extension
require 'test/unit'

class LoadTest < Test::Unit::TestCase
  def test_loading
    require 'yui'
    assert true
  end
end
