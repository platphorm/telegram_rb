module Telegram
  class User
    attr_accessor :id, :type, :name, :phone

    def to_peer
      Telegram::PeerId.new(type, id)
    end

    def send_message(message)
      Telegram.send_message(to_peer, message)
    end

  end

end
