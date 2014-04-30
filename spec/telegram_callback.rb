module Telegram

  def self.receive_message(message)
    p message.inspect
    message.mark_read
  end

end
