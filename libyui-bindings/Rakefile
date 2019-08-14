# -*- coding: utf-8 -*-
# make continuous integration using rubygem-packaging_rake_tasks and
# rubygem-libyui-rake.
# Copyright © 2014 SUSE LLC
# MIT license

require "libyui/rake"

Libyui::Tasks.submit_to :sle12sp5

Libyui::Tasks.configuration do |conf|
  # lets ignore license check for now
  conf.skip_license_check << /.*/
end
