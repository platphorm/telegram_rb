module Telegram
  class User
    attr_accessor :id, :type, :name, :phone

    def to_peer
      Telegram::PeerId.new(type, id)
    end

  end

end
