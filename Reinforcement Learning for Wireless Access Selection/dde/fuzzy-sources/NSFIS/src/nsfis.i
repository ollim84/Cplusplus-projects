%module nsfis
%{
#include "nsfis.h"
#include "fisAP.h"
#include "fisMN.h"
#include "fisNS.h"
%}
float nsfis(int op,float arg1,float arg2,float arg3);
