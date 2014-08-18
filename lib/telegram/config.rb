module Telegram
  class Config
    ROOT_DIR = File.expand_path(File.dirname(__FILE__) + '/../..')

    DEFAULTS = {
      schema: 'schema_14.json'
    }

    FIELDS = ['app_id', 'api_hash', 'public_key', 'test_ip', 'production_ip']

    class << self

      attr_accessor :schema, :config

      def load(file)
        raise TelegramError.new("Config file: #{file} not exist") unless File.exist?(file)

        self.config = YAML.load_file(file)
        self.schema = JSON.parse(File.open("#{ROOT_DIR}/config/#{DEFAULTS[:schema]}").read)
      end

    end
  end
end
