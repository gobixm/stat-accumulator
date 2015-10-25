{
  'targets': [
    {
      'target_name': 'bindings',
      'sources': [
        'sources/main.cc',
        'sources/accumulator.h',     
        'sources/accumulator.cc'   
      ],
      "include_dirs" : [
        '<!(node -e "require(\'nan\')")',
        "$(BOOST_ROOT)"        
      ],
      "libraries": [               
      ],
      "cflags_cc!": [ "-fno-rtti", "-fno-exceptions" ],
      "cflags!": [ "-fno-exceptions" ],
      'dependencies': [        
      ],
      'conditions':[
        ['OS=="win"', { 
        'defines':[
          'NOMINMAX'
        ]
        }]
      ],
      "repository": {
        "type": "git",
        "url": "git://github.com/gobixm/stat-accumulator.git"
      },
    }
  ]
}