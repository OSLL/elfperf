LIB_SOURCE_PATH=$(PWD)/test_lib
LIB_PATH=$(PWD)/libs
APP1_PATH=$(PWD)/test_dynamic_load
APP2_PATH=$(PWD)/test_dynamic_link

all: lib bin

lib: 
	gcc -g3 -fPIC -shared -o $(LIB_PATH)/libtest_lib.so $(LIB_SOURCE_PATH)/test_lib.c

test_dynamic_load_app: 
	gcc -g3 -rdynamic -o test_dynamic_load_app $(APP1_PATH)/test_dynamic_load_app.c -ldl

test_dynamic_link_app: 
	gcc -g3  -L$(LIB_PATH) -o test_dynamic_link_app $(APP2_PATH)/test_dynamic_link_app.c  -ltest_lib

bin: test_dynamic_link_app test_dynamic_load_app

clean:
	rm -f libs/*.so test_dynamic_load_app test_dynamic_link_app
