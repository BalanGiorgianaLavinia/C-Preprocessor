OBJ_LIST = utils.obj tema1.obj
CFLAGS = /nologo /W4 /EHsc /Za /Zi /MD /D_CRT_SECURE_NO_WARNINGS

build : so-cpp.exe

so-cpp.exe : $(OBJ_LIST)
	$(CPP) $(CFLAGS) /Fe$@ $**

clean : exe_clean obj_clean

obj_clean :
  del *.obj
 
exe_clean :
  del so-cpp.exe
 
