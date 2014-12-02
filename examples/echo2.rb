require 'telegram_rb'
trap("SIGINT") { exit! }

$myid = nil

module Telegram
  def self.receive_message(message)
    if message.to_id.id == 0
      $myid = message.from_id.id
      puts "My Id is "+$myid.to_s
    else
      if message.from_id.id != $myid
         Thread.new { Telegram.send_message(message.from_id,message.text) }
      end
    end
  end
end

Telegram.init

dummy = Telegram::PeerId.new(1, 0)

Telegram.send_message(dummy, "whoami")

while true
  Telegram.poll_messages
  sleep 5
end

