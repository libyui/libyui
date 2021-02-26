#--
# Copyright (C) 2015 SUSE LLC
#
# make continuous integration using rubygem-libyui-rake.
#
# MIT license
#++
require "libyui/rake"

Libyui::Tasks.configuration do |conf|
  conf.skip_license_check << /^Makefile\.repo$/
  conf.skip_license_check << /^src\/.*\.h\.in$/
  conf.skip_license_check << /^src\/icons\/.*\.svg$/
  conf.skip_license_check << /CMakeLists\.txt$/
  conf.skip_license_check << /.*\.qrc/
end
