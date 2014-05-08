module Telegram

  def self.receive_message(message)
  end

  def self.poll_messages_queue
  end

  def self.init(options = {})
    options[:key] ||= File.expand_path(File.dirname(__FILE__) + '/../../tg.pub') 
    options[:telegram_dir] = File.expand_path(options[:telegram_dir] || '.')

    unless Dir.exist?(options[:telegram_dir])
      Dir.mkdir(options[:telegram_dir])
    end

    TelegramRb::Config.download_folder = "#{options[:telegram_dir]}/.telegram/downloads"

    self.load_config(options[:key], options[:telegram_dir])
  end

end
