require 'telegram_rb'
trap("SIGINT") { exit! }

$flag = false
$peer = nil

module Telegram
  def self.receive_message(message)
    if $flag == false
      $flag = !$flag
      $peer = message.from_id
    else
      $flag = !$flag
      $peer = nil
    end
  end
end

Telegram.init

while true
  Telegram.poll_messages
  if $flag == true
    Telegram.send_message($peer, "Hello, TelegramRb")
  end
  sleep 5
end
