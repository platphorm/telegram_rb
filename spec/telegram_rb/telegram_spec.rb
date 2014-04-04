$:.unshift(File.dirname(__FILE__))
require 'spec_helper'
require 'pp'
  
describe "Telegram" do

  it 'int telegram' do
    Telegram.init
  end

  it 'print contact list' do
    Telegram.contact_list
  end

  it 'send message' do
    users = Telegram.contact_list
    user = users.find{|user| user.name == "Jiren_Patel"}
    user.send_message("From Ruby code #{rand(1000)}")
  end

  it 'receive messages' do
    puts "Send message to receive............"
    sleep(3)
    Telegram.poll_messages
  end



end

