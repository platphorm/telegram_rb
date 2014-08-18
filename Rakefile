require "bundler/gem_tasks"
require "rspec/core/rake_task"

$LOAD_PATH.unshift(File.dirname(__FILE__) + '/lib')

RSpec::Core::RakeTask.new("spec")

task :default => [:spec]
