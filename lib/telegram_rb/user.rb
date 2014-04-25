module Telegram
  class User
    attr_accessor :id, :type, :name, :phone

    def to_peer
      Telegram::PeerId.new(type, id)
    end

    # type = 0  Text message
    # type = 1  Photo
    # type = 1  Video

    def send_message(message)
      Telegram.send_message(to_peer, message, 0)
    end

    def send_photo(file_path)
      Telegram.send_message(to_peer, file_path, 1)
    end

    def send_video(file_path)
      Telegram.send_message(to_peer, file_path, 2)
    end

  end

end
