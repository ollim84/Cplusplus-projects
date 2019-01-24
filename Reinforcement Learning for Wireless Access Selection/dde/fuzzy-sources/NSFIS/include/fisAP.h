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
 * File:   fisAP.h
 * Author: hrivas
 *
 * Created on Oct 3, 2011, 3:20 PM
 */

#ifndef FISAP_H
#define	FISAP_H
//#include "fuzzylite/flScalar.h"
//using namespace fl;
class FisAP {
    public:
        //static flScalar FIS_AP(flScalar nu,flScalar lo, flScalar pl);
        static float FIS_AP(float nu, float lo, float pl);
        
    };


#endif	/* FISAP_H */

