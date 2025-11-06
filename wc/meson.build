project('woc', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

pkgconfig=import('pkgconfig') 
cc = meson.get_compiler('c') 

add_project_arguments(
  cc.get_supported_arguments(
    '-Wformat'
    ), 

  language:'c' 
  )

curses=dependency('curses') 


executable('woc',
           'woc.c',
           install : true)
