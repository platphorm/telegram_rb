require 'digest'

module TelegramRb
  module Mtproto
    def aes256_encrypt(key, data)
      key = Digest::SHA256.digest(key) if(key.kind_of?(String) && 32 != key.bytesize)
      aes = OpenSSL::Cipher.new('AES-256-CBC')
      aes.encrypt
      aes.key = key
      aes.update(data) + aes.final
    end

    def aes256_decrypt(key, data)
      key = Digest::SHA256.digest(key) if(key.kind_of?(String) && 32 != key.bytesize)
      aes = OpenSSL::Cipher.new('AES-256-CBC')
      aes.decrypt
      aes.key = key
      aes.update(data) + aes.final
    end
  end
end
