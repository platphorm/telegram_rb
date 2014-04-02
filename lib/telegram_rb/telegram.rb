module Telegram

  def self.receive_message(message)
    p message.inspect 
    #p message.from_id.inspect
    #Telegram.send_message(message.from_id, "From client: #{message.text}")
  end

end
