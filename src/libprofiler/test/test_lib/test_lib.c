#include <stdio.h>

int testFunction1(int arg1, int arg2)
{
  return arg1+arg2;

}

int testFunction2(){
  return 42;
}

void testFunction3(int arg1, int arg2){
  printf("TestFunction3, arg1=%d arg2=%d\n", arg1, arg2);
}

void testFunction4(){
  puts("TestFunction4");
}
