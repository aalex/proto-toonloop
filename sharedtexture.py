#!/usr/bin/python

import clutter
from clutter import  cogl


tex = cogl.texture_new_from_file("test.jpg")


s1 = clutter.Stage()
s1.set_size(100,100)
s1.show()

s2 = clutter.Stage()
s2.set_size(100,100)
s2.show()


t1 = clutter.Texture()
t1.set_cogl_texture(tex)
t1.set_size(100,100)
s1.add(t1)
s1.show()


t2 = clutter.Texture()
t2.set_cogl_texture(tex)
t2.set_size(100,100)
s2.add(t2)
s2.show()


clutter.main()
