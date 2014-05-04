module Telegram

  def self.receive_message(message)
    p "====== Received Message ======"
    p message.inspect
    message.mark_read
  end

  def self.poll_messages_queue
    p "*"*40  
=begin
    if @message_count.nil?
      @message_count = 1
      p "*"*40  
      users = Telegram.contact_list
      user = users.find{|user| ["Jiren_Patel", 'advisor'].include?(user.name)}
      p user
      user.send_message("From Ruby code #{rand(1000)}") if user
    end
=end
  end

end
