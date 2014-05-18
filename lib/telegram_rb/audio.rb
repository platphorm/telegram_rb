module Telegram
  class Audio
    attr_accessor :id, :duration, :date, :size

    def file_path(download_folder = nil)
      "#{TelegramRb::Config.download_folder}/download_#{id}"
    end

  end
end
