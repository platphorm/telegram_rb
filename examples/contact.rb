require 'telegram_rb'

Telegram.init

users = Telegram.contact_list
users.each do |u|
  puts "=================="
  puts u.name.to_s + " (" + u.id.to_s + ")"
  puts u.phone
end

puts "================="
