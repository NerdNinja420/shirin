project('shirin', 'cpp',
  version : '0.1.0',
  default_options : [
    'cpp_std=c++23',
    'warning_level=2',
    'optimization=3',
    'debug=true',
  ],
)

sdl3 = dependency('sdl3')

sources = files(
  'src/main.cpp',
  'src/math/vec2.cpp',
  'src/objects/player.cpp',
  'src/objects/scene.cpp',
  'src/render/game.cpp',
  'src/render/raycaster.cpp',
  'src/render/renderer.cpp',
  'src/utils/color.cpp',
)

executable('shirin', sources,
  dependencies : sdl3,
  include_directories : include_directories('include'),
  install : false,
)
