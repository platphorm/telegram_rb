$:.unshift(File.dirname(__FILE__))
require 'spec_helper'
require 'pp'
  
describe "Telegram" do

  it 'int telegram' do
    Telegram.init
  end

  it 'priint contact list' do
    Telegram.contact_list
  end

  it 'send message' do
    users = Telegram.contact_list
    user = users.find{|user| user.name == "Jiren_Patel"}
    Telegram.send_message(user.to_peer, "From Ruby code #{rand(1000)}")
  end

  it 'receive messages' do
    puts "Send message to receive............"
    sleep(3)
    Telegram.recv_messages
  end



end

