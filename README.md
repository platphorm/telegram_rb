# TelegramRb

TODO: Write a gem description

## Installation

Add this line to your application's Gemfile:

    gem 'telegram_rb'

And then execute:

    $ bundle

Or install it yourself as:

    # Prerequisites
    On ubuntu use:

       sudo apt-get install libreadline-dev libconfig-dev libssl-dev lua5.2 liblua5.2-dev
    
    On gentoo:

       sudo emerge -av sys-libs/readline dev-libs/libconfig dev-libs/openssl dev-lang/lua

    On Fedora:

       sudo yum install lua-devel openssl-devel libconfig-devel readline-devel

    On FreeBSD:

       pkg install libconfig libexecinfo lua52

    On OpenBSD:

       pkg_add libconfig libexecinfo lua
   
    On OS x 
        
       brew install libconfig
       brew install readline
       brew install lua
    
    
    $ gem install telegram_rb

## Usage

     # This will init telegram and ask for otp if mobile number is not registerd.
     Telegram.init
     
     # Fetch your contacts
     Telegram.contact_list
     
     # Send message
     users = Telegram.contact_list
     user = users.find{|user| user.phone == "1234567890"}
     Telegram.send_message(user.to_peer, "Hello from TelegramRb #{rand(1000)}")
     
     # Receive message implement callback
     module Telegram
       def self.receive_message(message)
         p message.inspect
       end
     end


## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
