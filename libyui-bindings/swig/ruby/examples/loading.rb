#
# Test loading of the bindings
#

$:.unshift "../../../build/swig/ruby"

# test loading of extension
require 'test/unit'

class LoadTest < Test::Unit::TestCase
  def test_loading
    require 'yui'
    assert true
  end
end
