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
      $msg = message.text.force_encoding("UTF-8")
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
    puts $msg.encoding.name
    Telegram.send_message($peer, $msg.reverse)
  end
  sleep 5
end
