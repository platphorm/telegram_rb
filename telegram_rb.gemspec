# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'telegram_rb/version'

Gem::Specification.new do |spec|
  spec.name          = "telegram_rb"
  spec.version       = TelegramRb::VERSION
  spec.authors       = ["Jiren Patel"]
  spec.email         = ["jiren@joshsoftware.com"]
  spec.description   = %q{Telegram ruby client}
  spec.summary       = %q{Telegram ruby client}
  spec.homepage      = ""
  spec.license       = "MIT"

  spec.files         = `git ls-files`.split($/)
  spec.executables   = spec.files.grep(%r{^bin/}) { |f| File.basename(f) }
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = ["lib"]
  spec.platform = Gem::Platform::RUBY
  spec.extensions << "ext/telegram_rb/extconf.rb"

  spec.add_development_dependency "rake-compiler"
  spec.add_development_dependency "bundler", "~> 1.3"
  spec.add_development_dependency "rake"
  spec.add_development_dependency "rspec"
  spec.add_dependency 'type_array'
end
