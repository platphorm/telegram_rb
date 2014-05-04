module Telegram
  class Message
    attr_accessor :id, :from_id, :to_id, :date, :text

    # Media Type : 1 -> text, 2 -> photo, 3 -> video, 4 -> audio

    attr_accessor :media_type, :media
  end
end
