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
 * Created on October 10, 2011, 2:29 PM
 */


#include <stdlib.h>
#include <vector>
//#include "fuzzylite/FuzzyLite.h"
#include "fisAP.h"
#include "fisMN.h"
#include "fisNS.h"
#include "nsfis.h"
#include <iomanip>
#include <fl/Headers.h>

using namespace fl;

 float nsfis(int op,float arg1,float arg2,float arg3) {
    //FL_LOG(std::fixed << std::setprecision(3));
    if (op==AP){
           float nu = arg1;
           float lo = arg2;
           float pl = arg3;
           return FisAP::FIS_AP(nu,lo,pl);
      }
       else
           if (op==MN){
              float ss = arg1;
              float ec = arg2;
              float bl = arg3;
              return FisMN::FIS_MN(ss, ec, bl);
          }
           else
               if (op==NS){
                  float ap = arg1;
                  float mn = arg2;
                  float bt = arg3;
                  return FisNS::FIS_NS(ap, mn, bt);
              }
               else
            	   if(op==NSVIDEO){
                       float ap = arg1;
                       float mn = arg2;
                       float bt = arg3;
                       return FisNS::FIS_NSVIDEO(ap, mn, bt);
            	   }
    
    return -1;
}

