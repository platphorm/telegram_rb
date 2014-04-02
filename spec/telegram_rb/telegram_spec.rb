$:.unshift(File.dirname(__FILE__))
require 'spec_helper'
require 'pp'
  
describe "Telegram" do

  it 'load cofig' do
    Telegram.load_config('tg.pub')
  end

  it 'send message' do
    peer = Telegram::PeerId.new(1, 32185523)
    Telegram.send_message(peer, "From Ruby code #{rand(1000)}")
  end

  it 'receive messages' do
    Telegram.recv_messages
  end

  it 'priint contact list' do
    Telegram.contact_list
  end

=begin
  it 'load config and send message' do
    Telegram.load_config('tg.pub')
    peer = Telegram::PeerId.new(1, 32185523)
    Telegram.send_message(peer, "From Ruby code #{rand(1000)}")
    sleep(2);
    Telegram.recv_messages
    pee1 = Telegram::PeerId.new(1, 36185322)
    Telegram.send_message(pee1, "From client: #{rand(4444)}")
    }
  end
=end


end

