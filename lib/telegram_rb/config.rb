require 'json'

module Telegram
  class Config

    SCHEMA_FILES = {
      schema_mtproto: 'schema_mtproto.json',
      schema_api: 'schema_api.json',
      emoji: 'emoji.json',
      emoji_categories: 'emoji_categories.json',
      emoji_category_spritesheet_dimens: 'emoji_category_spritesheet_dimens.json'
    }

    SCHEMA_API_LAYER = 10

    class << self

      SCHEMA_FILES.each{|k, _| attr_accessor k}

      def load(dir)
        SCHEMA_FILES.each do |k, file_name|
          file =  "#{dir}/#{file_name}"
          self.send("#{k}=", JSON.parse(IO.read(file)))
        end
      end

    end
  end
end
