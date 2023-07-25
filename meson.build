project(
  'icat',
  'c',
  version: '0.1.0',
  meson_version: '>-0.60.0',
  default_options : ['c_std=c2x', 'default_library=static']
)

cc = meson.get_compiler('c')

src = ['src/main.c']

lib_libc = cc.find_library('libc')

executable('icat', src, dependencies: lib_libc)
