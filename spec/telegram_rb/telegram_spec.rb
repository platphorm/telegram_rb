$:.unshift(File.dirname(__FILE__))
require 'spec_helper'
require 'pp'
  
describe "Telegram" do

=begin
  it 'int telegram' do
    Telegram.init
  end

  it 'print contact list' do
    Telegram.contact_list
  end

  it 'send message' do
    users = Telegram.contact_list
    user = users.find{|user| ["Jiren_Patel", 'advisor'].include?(user.name)}
    p user
    user.send_message("From Ruby code #{rand(1000)}") if user
  end

  it 'receive messages' do
    #puts "Send message to receive............"
    #sleep(3)
    #Telegram.poll_messages
  end
=end

  it 'start process' do
    Telegram.init
    Telegram.start
  end



end

