bin_PROGRAMS = toonloop 
# gtkshare

toonloop_SOURCES = main.cpp gltools.cpp draw.cpp gui.cpp pipeline.cpp gstgtk.cpp application.cpp
toonloop_CXXFLAGS = $(LIBM_CXXFLAGS) $(LIBTHREAD_CXXFLAGS)  $(LIBLO_CXXFLAGS) $(GST_CFLAGS) $(BOOST_CPPFLAGS) $(GL_CFLAGS) $(GLU_CFLAGS) $(GTKGLEXT_CFLAGS) 
toonloop_LDADD = $(LIBM_LIBS) $(LIBTHREAD_LIBS) $(LIBLO_LIBS) $(GST_LIBS) $(GSTAPP_LIBS) $(BOOST_LIBS)  $(BOOST_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LIB) $(GTKGLEXT_LIBS) 

