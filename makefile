#-----------------------------------------------------------------------------
# Definitions

C++	 = gcc
LD	 = $(C++)

OPTIMIZE = -g -fhandle-exceptions -DBOOL_IS_ALREADY_DEFINED -DREENTRANT
DEFINES	 = -w -fexternal-templates $(OPTIMIZE)
INCLUDES = -I.  #-I/pan/pan5/local/lib/g++-include/std
LIBS     = -lm -lnsl -g -lstdc++ -lg++ -lthread
LIBSRC   = libsrc #/home/bmajoros/c++/libsrc

LDFLAGS	 = #-L/usr/ccs/lib

#------------------------------------------------------------------------------
# Rules

obj/teststrstream.o: \
		teststrstream.C
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/teststrstream.o \
		teststrstream.C

obj/Exception.o: \
		libsrc/Exception.H \
		libsrc/Exception.C
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/Exception.o \
		libsrc/Exception.C

obj/RTTI.o: \
		libsrc/RTTI.H \
		libsrc/RTTI.C
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/RTTI.o \
		libsrc/RTTI.C

obj/identlst.o: \
		identlst.C \
		identlst.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/identlst.o \
		identlst.C

obj/ast.o: \
		ast.C \
		ast.H \
		tempmgr.H \
		execute.H \
		except.H \
		libsrc/linked2.H \
		libsrc/safecopy.H \
		symblrec.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/ast.o \
		ast.C

obj/astprint.o: \
		astprint.C \
		astprint.H  \
		class.H \
		visitor.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/astprint.o \
		astprint.C

obj/class.o: \
		object.H \
		symblrec.H \
		class.C \
		class.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/class.o \
		class.C

obj/cmdline.o: \
		cmdline.C \
		cmdline.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/cmdline.o \
		cmdline.C

obj/epsilon.o: \
		scanner.H \
		parser.H \
		except.H \
		libsrc/linked2.H \
		class.H \
		ast.H \
		$(LIBSRC)/array.C \
		tempmgr.H \
		execute.H \
		cmdline.H \
		epsilon.C
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/epsilon.o \
		epsilon.C

obj/except.o: \
		libsrc/safecopy.H \
		except.C \
		except.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/except.o \
		except.C

obj/execute.o: \
		execute.C \
		execute.H \
		class.H \
		object.H \
		ast.H \
		rtstack.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/execute.o \
		execute.C

obj/StringTokenizer.o: \
		/home/bmajoros/c++/libsrc/StringTokenizer.H \
		/home/bmajoros/c++/libsrc/StringTokenizer.C
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/StringTokenizer.o \
		/home/bmajoros/c++/libsrc/StringTokenizer.C

obj/garbage.o: \
		garbage.C \
		garbage.H \
		rtstack.H \
		cmdline.H \
		libsrc/linked2.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/garbage.o \
		garbage.C

obj/method.o: \
		method.C \
		method.H \
		ast.H \
		object.H \
		symblrec.H \
		execute.H \
		libsrc/linked2.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/method.o \
		method.C

obj/object.o: \
		object.C \
		object.H \
		rtstack.H \
		ast.H \
		except.H \
		execute.H \
		garbage.H \
		class.H \
		$(LIBSRC)/StringObject.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/object.o \
		object.C

obj/parser.o: \
		scanner.H \
		ast.H \
		libsrc/linked2.H \
		except.H \
		symblrec.H \
		parser.C \
		identlst.H \
		class.H \
		tempmgr.H \
		astprint.H \
		object.H \
		cmdline.H \
		libsrc/safecopy.H \
		parser.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/parser.o \
		parser.C

obj/tempmgr.o: \
		except.H \
		visitor.H \
		$(LIBSRC)/array.C \
		tempmgr.C \
		tempmgr.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/tempmgr.o \
		tempmgr.C

obj/symblrec.o: \
		libsrc/scopestk.H \
		libsrc/safecopy.H \
		method.H \
		symblrec.C \
		symblrec.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/symblrec.o \
		symblrec.C

obj/scanner.o: \
		except.H \
		scanner.H \
		scanner.C
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/scanner.o \
		scanner.C

obj/linked2.o: \
		libsrc/linked2.C \
		libsrc/linked2.H
	 $(C++) -c $(DEFINES) $(INCLUDES) -o obj/linked2.o \
		libsrc/linked2.C

obj/hash.o: \
		libsrc/hash.C \
		libsrc/hash.H
	 $(C++) -c $(DEFINES) $(INCLUDES) -o obj/hash.o \
		libsrc/hash.C

obj/StringObject.o: \
		$(LIBSRC)/StringObject.H \
		$(LIBSRC)/StringObject.C
	 $(C++) -c $(DEFINES) $(INCLUDES) -o obj/StringObject.o \
		$(LIBSRC)/StringObject.C

obj/Random.o: \
		libsrc/Random.C \
		libsrc/Random.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/Random.o \
		libsrc/Random.C

obj/rtstack.o: \
		ast.H \
		except.H \
		garbage.H \
		object.H \
		symblrec.H \
		rtstack.C \
		rtstack.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/rtstack.o \
		rtstack.C

obj/interpreter.o: \
		interpreter.C \
		interpreter.H \
		parser.H \
		scanner.H \
		except.H \
		libsrc/linked2.H \
		class.H \
		ast.H \
		$(LIBSRC)/array.C \
		tempmgr.H \
		execute.H \
		cmdline.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/interpreter.o \
		interpreter.C

obj/epsclass.o: \
		epsclass.H \
		epsclass.C \
		class.H \
		parser.H \
		$(LIBSRC)/StringObject.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/epsclass.o \
		epsclass.C

teststrstream: \
		obj/teststrstream.o
	$(C++) $(LDFLAGS)  -o teststrstream \
		obj/teststrstream.o \
		$(LIBS)

epscomp.a: \
		obj/ast.o \
		obj/RTTI.o \
		obj/Exception.o \
		obj/astprint.o \
		obj/class.o \
		obj/except.o \
		obj/execute.o \
		obj/interpreter.o \
		obj/garbage.o \
		obj/method.o \
		obj/object.o \
		obj/parser.o \
		obj/rtstack.o \
		obj/scanner.o \
		obj/StringObject.o \
		obj/linked2.o \
		obj/Random.o \
		obj/hash.o \
		obj/StringTokenizer.o \
		obj/identlst.o \
		obj/symblrec.o \
		obj/tempmgr.o
	ar rv epscomp.a \
		obj/ast.o \
		obj/RTTI.o \
		obj/astprint.o \
		obj/class.o \
		obj/except.o \
		obj/execute.o \
		obj/garbage.o \
		obj/StringObject.o \
		obj/linked2.o \
		obj/hash.o \
		obj/method.o \
		obj/object.o \
		obj/parser.o \
		obj/StringTokenizer.o \
		obj/rtstack.o \
		obj/scanner.o \
		obj/symblrec.o \
		obj/Random.o \
		obj/identlst.o \
		obj/Exception.o \
		obj/interpreter.o \
		obj/tempmgr.o

obj/testcomp.o: \
		testcomp.C \
		testcomp.H
	$(C++) -c $(DEFINES) $(INCLUDES) -o obj/testcomp.o \
		testcomp.C

testcomp: \
		obj/testcomp.o \
		obj/cmdline.o \
		epscomp.a
	$(C++) $(LDFLAGS)  -o testcomp \
		obj/cmdline.o \
		obj/testcomp.o \
		epscomp.a \
		$(LIBS)

epsilon: \
		obj/epsilon.o \
		obj/cmdline.o \
		epscomp.a
	$(C++) $(LDFLAGS)  -o epsilon \
		obj/epsilon.o \
		obj/cmdline.o \
		epscomp.a \
		$(LIBS)


