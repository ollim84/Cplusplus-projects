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
 * File:   fisNS.h
 * Author: hrivas
 *
 * Created on Oct 3, 2011, 3:25 PM
 */

#ifndef FISNS_H
#define	FISNS_H
//#include "fuzzylite/flScalar.h"

class FisNS {
    public:
        //static flScalar FIS_NS(flScalar ap,flScalar mn, flScalar bt);
        static float FIS_NS(float ap,float mn, float bt);
        static float FIS_NSVIDEO(float ap,float mn, float bt);
        
    };



#endif	/* FISNS_H */

