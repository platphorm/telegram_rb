require 'json'
require 'yaml'

require 'telegram/version'
require 'telegram/config'
require 'telegram/error'
require 'telegram/message'
require 'telegram/peer_id'
require 'telegram/number_util'
require 'telegram/serialization'

module Telegram

  def self.load_config(file)
    Telegram::Config.load(file)
  end

  class << self
    Telegram::Config::FIELDS.each do |f|
      define_method f do
        Telegram::Config.config[f]
      end
    end
  end

end
