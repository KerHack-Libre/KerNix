# Copyright(c) 2025  Umar Ba <jUmarB@protonmail.com>  @KerHack-Libre
project('cls', 'c',
  version : '1.2.1',
  license:['GPL-2.0-or-later'],
  meson_version:'>=1.6.1',
  default_options : ['c_std=gnu99','warning_level=3'])

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

install_man(['./docs/manpage/cls.1']) 
executable('cls',
           'cls.c',
           dependencies:[curses], 
           install : true)
