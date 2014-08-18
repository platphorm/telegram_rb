$:.unshift(File.dirname(__FILE__))
require 'spec_helper'

describe "Telegram" do

  it 'load config' do
    Telegram.load_config('config/config.yml')
    Telegram::Config::FIELDS.each do |f|
      expect(Telegram.send(f)).not_to be_nil
    end
  end

end

