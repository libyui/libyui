#--
# Copyright (C) 2015 SUSE LLC
#
# make continuous integration using rubygem-libyui-rake.
#
# MIT license
#++
require "libyui/rake"

Libyui::Tasks.submit_to :sle15sp2

Libyui::Tasks.configuration do |conf|
  # do not check license in qt metadata
  conf.skip_license_check << /.*\.qrc/
end
