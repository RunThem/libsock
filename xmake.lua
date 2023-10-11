--- Project name
set_project('libsock')

--- Project version
set_version('1.0.0')

--- xmake configure
set_xmakever('2.6.1')

--- Build mode
add_rules('mode.debug', 'mode.release')

--- Macro definition
add_defines('_GNU_SOURCE=1')

--- No warning
set_warnings('all', 'error')

--- Language standard
set_languages('c99')

--- Project common header file path
add_includedirs('$(projectdir)/src')

--- main target
target('libsock', function()
  set_kind('static')
  add_files('src/libsock.c')
  add_headerfiles('src/libsock.h')
end)

target('server', function()
  set_kind('binary')
  set_default(false)
  add_files('test/server.c')

  add_deps('libsock')
end)

target('client', function()
  set_kind('binary')
  set_default(false)
  add_files('test/client.c')

  add_deps('libsock')
end)
