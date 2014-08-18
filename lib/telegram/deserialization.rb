module Telegram
  class Deserialization

    attr_accessor :buffer, :offset

    def initialize(buffer, options = {})
      @buffer = buffer
      @offset = 0
      @mtproto = options[:mtproto] || false
    end

    def int_view
      @buffer.pack('c*').unpack('l*')
    end

    def read_int(field = nil)
      raise 'Nothing to fetch' if offset >= int_view * 4

      i = @buffer[offset..(offset + 4)].pack('c*').unpack('l*')
      self.offset += 4
      return i
    end

    def fetch_int(field = '')
      read_int("#{field}:int")
    end

    def fetch_double
      raise 'Need to be implemented'
    end

    def fetch_long(field = '')
      i_low = read_int("#{field}:long['low']")
      i_high = read_int("#{field}:long['high']")

      ((i_high << 32) + i_low).to_s
    end

    def fetch_bool(field = '')
      i = this.read_int("#{field}:bool")

      if i == 0x997275b5
        return true
      elsif i == 0xbc799737
        return false
      end

      raise "Unknown Bool constructor #{i}"
    end

    def fetch_string(field = '')
      len = @buffer[self.offset]
      self.offset += 1

      if len == 254
        arr = 3.times.collect{|i| b = @buffer[offset]; self.offset += 1; b}
        len = arr[0] | arr[1] << 8 | arr[2] << 16
      end

      s_utf8 = ''
      len.times do
        s_utf8 += @buffer[offset].chr
        self.offset += 1
      end

      (offset % 4).times{ self.offset += 1}

      URI.decode(URI.escape(s_utf8))
    end

    def fetch_bytes(field = '')
      len = @buffer[self.offset]
      self.offset += 1

      if len == 254
        arr = 3.times.collect{|i| b = @buffer[offset]; self.offset += 1; b}
        len = arr[0] | arr[1] << 8 | arr[2] << 16
      end

      bytes = []
      len.times do
        bytes << @buffer[offset].chr
        self.offset += 1
      end

      (offset % 4).times{ self.offset += 1}

      bytes
    end

    def fetch_int_bytes(bits, field = '')
      raise "Invalid bits: #{bits}" if (bits % 32) != 0 

      len = bits / 8

      bytes = []
      len.times do
        bytes << @buffer[offset].chr
        self.offset += 1
      end

      return bytes
    end

    def fetch_raw_bytes(len, field = '')
      len = read_int("#{field}_length") unless len

      bytes = []
      len.times do
        bytes << @buffer[offset].chr
        self.offset += 1
      end

      bytes
    end

    def fetch_object(type, field = '')
      case type
      when 'int'
        return fetch_int(obj, field)
      when 'long'
        return fetch_long(obj, field)
      when 'int128'
        return fetch_int_bytes(obj, 128, field)
      when 'int256'
        return fetch_int_bytes(obj, 256, field)
      when 'int512'
        return fetch_int_bytes(obj, 512, field)
      when 'string'
        return fetch_string(obj, field)
      when 'bytes'
        return fetch_bytes(obj, field)
      when 'double'
        return fetch_double(obj, field)
      when 'Bool'
        return fetch_bool(obj, field)
      end

      field = field || type || 'Object'

      if ['vector', 'Vector'].include?(type[0..6])
        if type[0] == 'V'
          constructor = read_int("#{field}[id]")
          raise "Invalid vector constructor #{constructor}" if constructor != 0x1cb5c415
        end
        
        len = read_int("#{field}[count]")
        result = []
        if len > 0
          item_type = type[7..(type.length - 8)]
          len.times{|i| fetch_object(item_typem "#{field}[#{i}]") }
        end
        return result
      end

      schema = mtproto ? Config.schema_mtproto : Config.schema_api
      predicate = false
      constructor_data = false

      if type[0] == '%'
        check_type = type[1..-1]
        unless constructor_data = schema['constructors'].find { |c| c['type'] == check_type }
          raise "'Constructor not found for type: #{type}"
        end
      elsif type.getbyte(0) >= 97 && type.getbyte(0) <= 122
        unless constructor_data = schema['constructors'].find { |c| c['predicate'] == type }
          raise "Constructor not found for predicate: #{type}"
        end
      else
        constructor = read_int("#{field}[id]")
        constructor_cmp = unit_to_int(constructor)

        if constructor_cmp == 0x3072cfa1
          compressed = fetch_bytes("#{field}[packed_string]")
          uncompressed = gzipUncompress(compressed)
          buffer = bytesToArrayBuffer(uncompressed)
          deserializer = Deserialization.new(buffer)

          return deserializer.fetch_object(type, field);
        end

        unless constructor_data = schema['constructors'].find { |c| c['predicate'] == type }
          raise "Constructor not found for predicate: #{type}"
        end

        
      end

    end

  end
end
