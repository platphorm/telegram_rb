require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

$LOAD_PATH.unshift(File.dirname(__FILE__) + '/lib')

Rake::ExtensionTask.new do |ext|
  ext.name = "telegram_ext"
  ext.ext_dir = "ext/telegram_rb"
  ext.lib_dir = "lib"
end
RSpec::Core::RakeTask.new("spec")
  
desc 'clean all extension files'
task :clean_all => :clean do
  begin
    Dir.chdir(Pathname(__FILE__).dirname + "lib") do
      `rm telegram_rb.#{extension}`
      `rm telegram_rb.o`
    end
  rescue Exception => e
    puts e.message
  end
end

task :default => [ :clean_all, :compile, :spec ]
#task :default => [:spec]
