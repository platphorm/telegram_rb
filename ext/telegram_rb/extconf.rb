require 'mkmf'

extension_name = 'telegram_ext'
#dir_config("#{extension_name}/#{extension_name}")

HEADER_DIRS = ["inc"]
LIB_DIRS = ["libs"]
dir_config("libs", HEADER_DIRS, LIB_DIRS)

#RbConfig::MAKEFILE_CONFIG['CC'] = 'gcc'
RbConfig::MAKEFILE_CONFIG['CCDLFLAGS'] = ''
#$CFLAGS << " -DHAVE_CONFIG_H -Wall -Wextra -Werror -Wno-deprecated-declarations -fno-strict-aliasing -fno-omit-frame-pointer "
$CFLAGS << " -DHAVE_CONFIG_H -Wall -Wextra -Wno-deprecated-declarations -fno-strict-aliasing -fno-omit-frame-pointer"

have_library('tgl')
have_library('config')
have_library('z')
have_library('m')
have_library('crypto')

create_makefile(extension_name)
