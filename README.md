# TelegramRb

This is the first cut of the rubygem for telegram. A lot of the native code here is picked up from telegram client. However, it should be rewritten as a proper library and not as a CLI port. Since this has been ported from the CLI, there have been a lot of global variables used and it causes problems in the library if we want to multiplex telegram messages i.e. send and receive messages in parallel. I am adding a work-around for this for now and it is good enough for basic testing from IRB.

## Installation Prerequisites (for Telegram)

On ubuntu use:

    sudo apt-get install libconfig-dev libssl-dev
    
On gentoo:

    sudo emerge -av dev-libs/libconfig dev-libs/openssl

On Fedora:

    sudo yum install openssl-devel libconfig-devel 

On FreeBSD:

    pkg install libconfig libexecinfo

On OpenBSD:

    pkg_add libconfig libexecinfo
   
On OS x 
        
    brew install libconfig
    
## Installation 

Add this line to your application's Gemfile:

    gem 'telegram_rb'

And then execute:

    $ bundle
    

## Trouble-shooting during installation. 

If while installing you get issues with native compilations, god help you. Here are some pointers though.

1.  `ld: 35 duplicate symbols for architecture x86_64`
    
    This is probably because of the compiler setting `-fno-common`. Open your rbconfig.rb file. This typically resides inside `<RUBY PATH>/lib/ruby/<version>/<architecture>/rbconfig.rb`. For example: `~/.rvm/rubies/ruby-2.1.0/lib/ruby/2.1.0/x86_64-darwin13.0/rbconfig.rb`.
    
    In this file, edit `CONFIG["CFLAGS"]` and remove -fno-common

2.  `cc1: error: unrecognized command line option "-Wdivision-by-zero"`
    
    In this file, edit `CONFIG["warnflags"]` and remove `-Wdivision-by-zero` from the warnings.

## Usage

```ruby
# This will init telegram and ask for otp if mobile number is not registerd.
Telegram.init
     
# At this point, you will be asked to provide a Telephone number and an SMS code will be sent to you. 
# This will be done only ONCE when you first init. The next time init will pick up your configuration
# from the files saved in .telegram folder.
# 
# You should see something like this:
#   [~/.telegram] created
#   [~/.telegram/downloads] created
#   Telephone number (with '+' sign): <add your phone number here>
#   *** phone registered
#   *** sending code
#   *** send_code: dc_num = 4
#   Code from sms (if you did not receive an SMS and want to be called, type "call"): 
#     => nil
     
# Fetch your contacts
Telegram.contact_list
     
# Send message
users = Telegram.contact_list
user = users.find{|user| user.phone == "1234567890"}
Telegram.send_message(user.to_peer, "Hello from TelegramRb #{rand(1000)}")
     
# Receive message callback
module Telegram
  def self.receive_message(message)
    p message.inspect
  end
end

# Poll messages: this will get messages and call `receive_message` method
Telegram.poll_messages

```

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
