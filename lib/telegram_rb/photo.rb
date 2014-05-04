module Telegram
  class Photo
    attr_accessor :w, :h, :volume, :local_id, :size

    def file_path(download_folder = nil)
      "#{TelegramRb::Config.download_folder}/download_#{volume}_#{local_id}"
    end

  end
end
