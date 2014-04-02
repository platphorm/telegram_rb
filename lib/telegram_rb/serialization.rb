#require 'type_array'

module TelegramRb
  class Serialization
    include NumberUtil

    attr_accessor :max_length, :offset, :mtproto
    attr_accessor :buffer, :int_view

    def initialize(options = {})
      @max_length = options[:start_max_length] || 2048 # 2kb
      @offset = 0 # in bytes
      #@buffer = Array.new(@max_length, 0)
      @buffer = []

      @mtproto = options[:mtproto] || false
    end

    def get_array
      @buffer[0..(offset/4)]
    end

    def int_view
      @buffer.pack('c*').unpack('l*')
    end

    def get_buffer
      get_array.pack('l*')
    end

    def get_bytes
      get_array.pack('l*').bytes
    end

    def check_length(need_bytes)
      return nil if (offset + need_bytes) < max_length

      self.max_length = ([max_length * 2, offset + need_bytes + 16].max / 4)*4
      previous_buffer = buffer
      previous_array = Int32Array.new(previous_buffer)

      Int32Array.new(self.buffer).set(previous_array)
    end

    def write_int(i, field)
      #puts "#{i} : #{field}"
      bytes = [i].pack('l').bytes
      self.buffer.concat(bytes)
      self.offset += 4
    end

    def store_int(i, field = nil)
      write_int(i, (field || '') + ':int')
    end

    def store_bool(i, field = nil)
      write_int(i ? 0x997275b5 : 0xbc799737, (field || '') + ':bool')
    end

    def store_long_p(i_high, i_low, field = nil)
      write_int(i_low, (field || '') + ':long[low]')
      write_int(i_high, (field || '') + ':long[high]')
    end

    def store_long(s_long, field = nil)
      if s_long.is_a?(Array)
        if s_long.length == 2
          return store_long_p(s_long.first, s_long.last, field)
        else
          return store_int_bytes(s_long, 64, field)
        end
      end

      div_rem = s_long.divmod(0x100000000)
      write_int(int_to_uint(div_rem[1]), (field || '') + ':long[low]')
      write_int(int_to_uint(div_rem[0]), (field || '') + ':long[high]')
    end

    # NOTE: not in use
    def store_double(f, field = nil)
      buffer = ArrayBuffer.new(8)
      int_view = Int32Array(buffer)
      double_view = Float64Array.new(buffer)

      double_view[0] = f
      write_int(int_view[0], (field || '') + ':double[low]')
      write_int(int_view[1], (field || '') + ':double[high]')
    end

    def store_string(s, field = nil)
      s_utf8 = URI.unescape(URI.encode(s))
      len = s_utf8.length

      if len <= 253
        @buffer[self.offset] = len
        self.offset += 1
      else
        @buffer[self.offset] = 254
        self.offset += 1
        @buffer[self.offset] = len & 0xFF
        self.offset += 1
        @buffer[self.offset] = (len & 0xFF00) >> 8
        self.offset += 1
        @buffer[self.offset] = (len & 0xFF0000) >> 16
        self.offset += 1
      end

      s_utf8.each_byte{|b| @buffer[offset] = b; self.offset += 1 }

      (offset % 4).times { @buffer[offset] = 0; self.offset += 1 }
    end

    def store_bytes(bytes, field)
      len = bytes.length

      if len <= 253
        @buffer[offset] = len
        self.offset += 1
      else
        @buffer[offset] = 254
        self.offset += 1
        @buffer[offset] = len & 0xFF
        self.offset += 1
        @buffer[offset] = (len & 0xFF00) >> 8
        self.offset += 1
        @buffer[offset] = (len & 0xFF0000) >> 16
        self.offset += 1
      end

      len.times{|i| @buffer[self.offset += 1] = bytes[i]}
      (offset % 4).times { @buffer[self.offset += 1] = 0 }
    end

    def store_int_bytes(bytes, bits, field = nil)
      len = bytes.length
      raise ("Invalid bits: #{bits} , #{bytes.length}") if ((bits % 32) != 0 || (len * 8) != bits)

      bytes.each{|b| self.buffer[self.offset] = b; self.offset += 1}
    end

    def store_raw_bytes(bytes, field = nil)
      bytes.each{|b| self.buffer[self.offset] = b; self.offset += 1}
    end

    def store_method(method_name, params)
      schema = self.mtproto ? Config.schema_mtproto : Config.schema_api
      method_data = false

      method_data = schema['methods'].find{|m| m['method'] = method_name}

      raise "No method #{method_name}  found"

      store_int(int_to_uint(method_data['id']), "#{method_name}[id]")

      method_data['params'].each do |name, type|
        store_object(name, type, "#{method_name}[#{name}]")
      end
    end

    def store_object(obj, type, field)
      case type
      when 'int'
        return store_int(obj, field)
      when 'long'
        return store_long(obj, field)
      when 'int128'
        return store_int_bytes(obj, 128, field)
      when 'int256'
        return store_int_bytes(obj, 256, field)
      when 'int512'
        return store_int_bytes(obj, 512, field)
      when 'string'
        return store_string(obj, field)
      when 'bytes'
        return store_bytes(obj, field)
      when 'double'
        return store_double(obj, field)
      when 'Bool'
        return store_bool(obj, field)
      end

      if obj.is_a?(Array)
        if(type[0..6]) == 'Vector'
          write_int(0x1cb5c415, "#{field}[id]")
        elsif type[0..6] != 'vector'
          raise "Invalid vector type #{type}"
        end

        item_type = type[7..(type.length - 8)]
        write_int(obj.length, "#{field}[count]")

        obj.length.times{|i| store_object(obj[i], item_type, "#{field}[#{i}]") }

        return true
      elsif type[0..6].downcase == 'vector'
        raise "Invalid vector object"
      end

      if obj.is_a?(Hash)
        raise "Invalid vector type #{type}"
      end

      schema = self.mtproto ? Config.schema_mtproto : Config.schema_api
      predicate = obj['_']
      is_bare = false
      constructor_data = false

      type = type[1..-1] if is_bare = (type[0] == '%') 
      unless constructor_data = schema['constructors'].find { |c| c['predicate'] == predicate }
        raise "No predicate #{predicate} found"
      end

      is_bare = true if predicate == type
      unless is_bare
        write_int(int_to_uint(constructor_data['id']), "#{field}[#{predicate}][id]")
      end

      constructor_data['params'].each do |name, type|
        store_object(name, type, "#{field}[#{predicate}][#{name}]")
      end

    end

  end
end
