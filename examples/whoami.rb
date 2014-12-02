require 'telegram_rb'

$myid = nil

module Telegram
  def self.receive_message(message)
    if message.to_id.id == 0
      $myid = message.from_id.id
    end
  end
end

Telegram.init

dummy = Telegram::PeerId.new(1, 0)

Telegram.send_message(dummy, "whoami")

while $myid == nil
  Telegram.poll_messages
  sleep 5
end

puts "My ID is "+$myid.to_s
