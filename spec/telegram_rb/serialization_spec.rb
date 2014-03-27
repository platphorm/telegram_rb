$:.unshift(File.dirname(__FILE__))
require 'spec_helper'
  
module Telegram 

  describe 'Serialization' do

    before(:each){ @s = Serialization.new }

    it 'init object with default values' do
      @s = Serialization.new
      expect(@s.max_length).to eq 2048
      expect(@s.mtproto).to eq false
    end

    describe "#offset_zero" do

      it 'return empty array' do
        expect(@s.get_array).to be_empty
      end

      it 'return empty byte array' do
        expect(@s.get_bytes).to be_empty
      end

    end

    it 'write int to buffer' do
      @s.write_int(10, 'int')
      expect(@s.offset).to eq 4
    end

    it 'store int' do
      @s.store_int(100000)
      expect(@s.buffer).to eq([160, 134, 1, 0])
      expect(@s.offset).to eq 4
    end

    it 'store boolean' do
      @s.store_bool(true)
      @s.store_bool(false)
      expect(@s.buffer).to eq([181, 117, 114, 153, 55, 151, 121, 188])
    end

    it 'store long number' do
      @s.store_long(6000000000)
      expect(@s.buffer).to eq([0, 188, 160, 101, 1, 0, 0, 0])
    end

    it 'store long number using array' do
      @s.store_long([100000, 200000])
      expect(@s.buffer).to eq([64, 13, 3, 0, 160, 134, 1, 0])
    end

    it 'store long number using bytes' do
      @s.store_long([244, 1, 0, 0, 1,200, 1, 0])
      expect(@s.buffer).to eq([244, 1, 0, 0, 1,200, 1, 0])

      expect(@s.int_view).to eq([500, 116737])
    end

    it 'store int bytes' do
      @s.store_int_bytes([244, 1, 0, 0], 32)
      expect(@s.buffer).to eq([244, 1, 0, 0])
      expect(@s.int_view).to eq([500])
    end

    it 'store string' do
      @s.store_string('hi this is test')
      expect(@s.offset).to eq 16
      expect(@s.buffer).to eq([15, 104, 105, 32, 116, 104, 105, 115, 32, 105, 115, 32, 116, 101, 115, 116])
      expect(@s.int_view).to eq([543778831, 1936287860, 544434464, 1953719668])
    end



  end

end
