LIB_SOURCE_PATH=$(PWD)/test_lib
LIB_PATH=$(PWD)/libs
APP1_PATH=$(PWD)/test_dynamic_load
APP2_PATH=$(PWD)/test_dynamic_link

all: lib bin

lib: 
	gcc -g3 -m32  -shared  $(LIB_SOURCE_PATH)/test_lib.c -o $(LIB_PATH)/libtest_lib.so

test_dynamic_load_app: 
	gcc -m32 -g3 -rdynamic -o test_dynamic_load_app $(APP1_PATH)/test_dynamic_load_app.c -ldl

test_dynamic_link_app: 
	gcc  -m32 -g3  -L$(LIB_PATH) -o test_dynamic_link_app $(APP2_PATH)/test_dynamic_link_app.c  -ltest_lib

bin: test_dynamic_link_app test_dynamic_load_app

clean:
	rm -f libs/*.so test_dynamic_load_app test_dynamic_link_app
