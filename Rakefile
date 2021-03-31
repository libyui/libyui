#--
# Copyright (c) 2015-2019 SUSE LLC
# Copyright (c) 2020-2021 SUSE LLC
#
# make continuous integration using rubygem-libyui-rake.
#
# MIT license
#++
require "libyui/rake"

Libyui::Tasks.submit_to :sle15sp3

Libyui::Tasks.configuration do |conf|
  include Libyui::Tasks::Helpers

  conf.skip_license_check << /_multibuild/
  conf.skip_license_check << /^ci-tasks$/
  conf.skip_license_check << /^Dockerfile$/
  conf.skip_license_check << /legacy-buildtools\/.*/
  conf.skip_license_check << /Makefile\.repo$/
  conf.skip_license_check << /CMakeLists\.txt$/
  conf.skip_license_check << /src\/.*\.h\.in$/
  conf.skip_license_check << /src\/icons\/.*\.svg$/
  conf.skip_license_check << /^libyui\/pkgconfig\/.*\.pc\.in$/
  conf.skip_license_check << /^libyui-bindings\/.*/
  conf.skip_license_check << /^libyui-qt\/src\/lang_fonts/
end
