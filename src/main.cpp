#include<stdlib.h>
#include<stdio.h>
#include<iostream>


#include "Core/AppBuilder.h"


int main(int argc, char *argv[]){
    Application * app = AppBuilder::MakeApp(argc, argv);
    app->Run();

	return 0;
}
