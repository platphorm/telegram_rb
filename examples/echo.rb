require 'telegram_rb'
trap("SIGINT") { exit! }

$flag = false
$peer = nil
$msg = ""

module Telegram
  def self.receive_message(message)
    if $flag == false
      $flag = !$flag
      $peer = message.from_id
      $msg = message.text
    else
      $flag = !$flag
      $peer = nil
      $msg = ""
    end
  end
end

Telegram.init

while true
  Telegram.poll_messages
  if $flag == true
    Telegram.send_message($peer, $msg)
  end
  sleep 5
end
