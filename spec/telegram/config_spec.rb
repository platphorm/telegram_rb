$:.unshift(File.dirname(__FILE__))
require 'spec_helper'

module Telegram

  describe "Config" do

    it 'load scheam' do
      Config.load(SCHEMA_DIR)

      Config::SCHEMA_FILES.each do |k, file|
        expect(Config.send(k)).not_to be_nil
      end
    end
  end

end
