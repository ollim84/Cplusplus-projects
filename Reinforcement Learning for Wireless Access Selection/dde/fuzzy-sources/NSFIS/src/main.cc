/*   Copyright 2011 Helena Rivas

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */
/* 
 * File:   main.cpp
 * Author: hrivas
 *
 * Created on October 3, 2011, 2:29 PM
 */

#include <stdlib.h>
#include <vector>
//#include "fuzzylite/FuzzyLite.h"
#include "fisAP.h"
#include "fisMN.h"
#include "fisNS.h"
#include <stdio.h>
#include <iomanip>
#include <string>
#include <fl/Headers.h>
using namespace std;
using namespace fl;
int main(int argc, char** argv) {
    //FL_LOG(std::fixed << std::setprecision(3));
    if (argc != 5) {
        printf("usage: fuzzyliteFIS -ap|-mn|-ns a1 a2 a3\n");
        printf("-ap: Access Point Classification\n");
        printf("-mn: Mobile Node Classification\n");
        printf("-ns: Network Selection Classification\n");

    }else{
       string arg = argv[1];

       if (arg=="-ap"){
           float nu = atof( argv[2] );
           float lo = atof( argv[3] );
           float pl = atof( argv[4] );
           printf("%f\n",FisAP::FIS_AP(nu,lo,pl));
      }
       else
           if (arg=="-mn"){
              float ss = atof( argv[2] );
              float ec = atof( argv[3] );
              float bl = atof( argv[4] );
              printf("%f\n",FisMN::FIS_MN(ss, ec, bl));
          }
           else
               if (arg=="-ns"){
                  float ap = atof( argv[2] );
                  float mn = atof( argv[3] );
                  float bt = atof( argv[4] );
                  printf("%f\n",FisNS::FIS_NS(ap, mn, bt));
              }
               else
            	   if(arg=="-nsvideo") {
                       float ap = atof( argv[2] );
                       float mn = atof( argv[3] );
                       float bt = atof( argv[4] );
                       printf("%f\n",FisNS::FIS_NSVIDEO(ap, mn, bt));
            	   }
    }

}

