
project('cls', 'c',
  version : '1.0',
  default_options : ['warning_level=3'])

pkg_config=import('pkgconfig') 

cc = meson.get_compiler('c') 

add_project_arguments(
  cc.get_supported_arguments(
    '-Wformat',
    '-Wunused'
  ), 
  '-DCLS_VERSION='+  meson.project_version(),  
  language:'c',
  )

curses=dependency('curses')  




executable('cls',
           'cls.c',
           dependencies:[curses], 
           install : true)
