module Telegram

  def self.receive_message(message)
  end

  def self.poll_messages_queue
  end

  def self.init(options = {})
    pub_key_path = options[:key] || File.expand_path(File.dirname(__FILE__) + '/../../tg.pub') 
    self.load_config(pub_key_path)
  end

end
