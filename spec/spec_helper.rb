require 'rubygems'
require 'bundler/setup'

RSpec.configure do |config|
  config.color_enabled = true
  #config.tty = true
  #config.formatter = :documentation
end

$:.unshift(File.dirname(__FILE__) + '/../lib/')

SCHEMA_DIR = "#{Dir.pwd}/schema"

require 'telegram'
