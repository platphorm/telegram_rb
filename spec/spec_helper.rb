require 'rubygems'
require 'bundler/setup'
require 'open-uri'
=begin
require 'simplecov'

SimpleCov.start do
  add_filter '/spec/'
  add_group 'gem', 'lib'
end
=end

RSpec.configure do |config|
  config.color_enabled = true
  #config.tty = true
  #config.formatter = :documentation
end

$:.unshift(File.dirname(__FILE__) + '/../lib/')

SCHEMA_DIR = "#{Dir.pwd}/schema"

require 'telegram_rb'
require "#{File.dirname(__FILE__)}/telegram_callback" 
