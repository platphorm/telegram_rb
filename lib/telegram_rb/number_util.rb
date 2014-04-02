module TelegramRb
  module NumberUtil

    def to_byte_array(num)
      result = []
      begin
        result << (num & 0xff)
        num >>= 8
      end until (num == 0 || num == -1) && (result.last[7] == num[7])
      result.reverse
    end

    def int_to_uint(val)   
      val = val.to_i;
      val += 4294967296 if val < 0
      val
    end

    def uint_to_int(val)
      val -= 4294967296 if val < 2147483647
      val
    end

  end
end
