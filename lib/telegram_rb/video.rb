module Telegram
  class Video
    attr_accessor :id, :w, :h, :duration, :size

    def file_path(download_folder = nil)
      "#{TelegramRb::Config.download_folder}/download_#{id}"
    end

  end
end
