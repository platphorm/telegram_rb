$:.unshift(File.dirname(__FILE__))
require 'spec_helper'
  
module Telegram 

  describe Config do

    it 'load scheam' do
      Telegram::Config.load(SCHEMA_DIR)

      Telegram::Config::SCHEMA_FILES.each do |k, file|
        expect(Telegram::Config.send(k)).not_to be_nil 
      end
    end
  end

end
