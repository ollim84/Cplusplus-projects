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
#include "fisNS.h"
//#include "fuzzylite/FuzzyLite.h"
#include <limits>
#include <cstdlib>
//#include "fuzzylite/FunctionTerm.h"
//#include <omnetpp.h>
#include <fl/Headers.h>

using namespace fl;

/**
 * v. 3.1 code
 */

float FisNS::FIS_NS(float apt,float mbn,float bt)
{


	// Declaration of the fuzzy engine
	fl::Engine* engine = new fl::Engine;
	engine->setName("Network Selection FIS-mamdani");

	//TODO:
//	engine->addHedge(new fl::Not);
//	engine->addHedge(new fl::Somewhat);
//	engine->addHedge(new fl::Very);


	// FIS_AP output goodness of AP
	fl::InputVariable* ap = new fl::InputVariable;
	ap->setName("AP");

	// 0 - 1
//	ap->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	ap->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	ap->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	ap->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	ap->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 - 5
	ap->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	ap->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	ap->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	ap->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	ap->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	engine->addInputVariable(ap);

	// FIS_MN output goodness of network interface
	fl::InputVariable* mn = new fl::InputVariable;
	mn->setName("MN");

	// 0 - 1
//	mn->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	mn->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	mn->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	mn->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	mn->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 - 5
	mn->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	mn->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	mn->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	mn->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	mn->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	engine->addInputVariable(mn);

	/** Cached content variable. If value = 0, CACHE = FALSE, value = 1, CACHE = TRUE **/
	fl::InputVariable* cache = new fl::InputVariable;
	cache->setName("CACHE");
	cache->addTerm(new fl::Ramp("FALSE", 0.500, 0.000));
	cache->addTerm(new fl::Ramp("TRUE", 0.500, 1.000));
	engine->addInputVariable(cache);

	// Buffer status of the video player
//	fl::InputVariable* buffer = new fl::InputVariable;
//	buffer->setName("BUFFER");
//	buffer->addTerm(new fl::Ramp("LOW", 15.000, 0.000));
//	buffer->addTerm(new fl::Triangle("MODERATE", 0.000, 15.000, 100.0));
//	buffer->addTerm(new fl::Ramp("HIGH", 15.000, 100.000));
//	engine->addInputVariable(buffer);



	// Network Selection FIS Classification
	fl::OutputVariable* nc = new fl::OutputVariable;
	nc->setName("Classification");

	// 0 - 1
//	nc->setRange(0.000, 1.000);
//	nc->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	nc->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	nc->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	nc->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	nc->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 -5
	nc->setRange(0.000, 5.000);
	nc->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	nc->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	nc->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	nc->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	nc->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	nc->setLockOutputValueInRange(true);
	nc->setDefaultValue(fl::nan);
	//nc->setLockValidOutput(true);
	nc->setDefuzzifier(new fl::Centroid(500));
	//nc->output()->setAccumulation(new fl::Maximum);
	nc->fuzzyOutput()->setAccumulation(new fl::Maximum);

	engine->addOutputVariable(nc);

    fl::RuleBlock* block = new fl::RuleBlock();
	block->setName("");
	block->setConjunction(new fl::Minimum);
	block->setDisjunction(new fl::Maximum);
	block->setActivation(new fl::Minimum);

	/*** General classifications ***/

	 block->addRule(fl::Rule::parse("if AP is BAD then Classification is BAD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  ( AP is REGULAR and MN is BAD )  then Classification is BAD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  ( AP is REGULAR and MN is REGULAR )  then Classification is BAD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  ( AP is GOOD and MN is BAD )  then Classification is BAD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  ( AP is VERY_GOOD and MN is BAD )  then Classification is BAD", engine)); //OK

	/*** Detailed classifications  ***/
	 //1.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK

	 //2.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK

	 //3.
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and CACHE is TRUE )  and AP is GOOD )  then Classification is REGULAR", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and CACHE is FALSE )  and AP is GOOD )  then Classification is REGULAR", engine)); //OK

	 //4.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is GOOD )  then Classification is REGULAR", engine)); //OK

	 //5.
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is REGULAR", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is REGULAR", engine)); //OK

	 //6.
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is REGULAR", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is REGULAR", engine)); //OK

	 //7.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is GOOD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is GOOD", engine)); //OK

	 //8.
	 block->addRule(fl::Rule::parse("if  (  ( MN is BAD and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is BAD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is BAD and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is BAD", engine)); //OK

	 //9.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK

	 //10.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK

	 //11.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine)); //OK

	 //12.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK

	 //13.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
     block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK

	 //14.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK

	 //15.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK

	 //16.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine)); //OK


     engine->addRuleBlock(block);

     //engine->configure("Minimum", "Maximum", "Minimum", "Maximum", "Centroid", FL_DIVISIONS);

     ap->setInputValue(apt);
     mn->setInputValue(mbn);
     cache->setInputValue(bt);

// 	std::cout << "AP Classification is: " << ap->fuzzify(apt) << "\n";
// 	std::cout << "MN Classification is: " << nc->fuzzify(mbn) << "\n";
// 	std::cout << "CACHE Classification is: " << cache->fuzzify(bt) << "\n";


     engine->process();
     nc->defuzzify();
     float out = 0.0;
     out = nc->getOutputValue();

     //float out = nc->defuzzify();


	//std::cout << "NS Classification is: " << nc->fuzzify(out) << "\n";


     return out;

}

float FisNS::FIS_NSVIDEO(float apt,float mbn,float bt)
{


	// Declaration of the fuzzy engine
	fl::Engine* engine = new fl::Engine;
	engine->setName("Network Selection FIS-mamdani");

//	engine->addHedge(new fl::Not);
//	engine->addHedge(new fl::Somewhat);
//	engine->addHedge(new fl::Very);


	// FIS_AP output goodness of AP
	fl::InputVariable* ap = new fl::InputVariable;
	ap->setName("AP");

	// 0 - 1
//	ap->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	ap->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	ap->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	ap->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	ap->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 - 5
	ap->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	ap->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	ap->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	ap->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	ap->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	engine->addInputVariable(ap);

	// FIS_MN output goodness of network interface
	fl::InputVariable* mn = new fl::InputVariable;
	mn->setName("MN");

	// 0 - 1
//	mn->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	mn->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	mn->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	mn->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	mn->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 - 5
	mn->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	mn->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	mn->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	mn->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	mn->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	engine->addInputVariable(mn);

	/** Cached content variable. If value = 0, CACHE = FALSE, value = 1, CACHE = TRUE **/
//	fl::InputVariable* cache = new fl::InputVariable;
//	cache->setName("CACHE");
//	cache->addTerm(new fl::Ramp("FALSE", 0.500, 0.000));
//	cache->addTerm(new fl::Ramp("TRUE", 0.500, 1.000));
//	engine->addInputVariable(cache);

	// Buffer status of the video player
	fl::InputVariable* buffer = new fl::InputVariable;
	buffer->setName("BUFFER");
	buffer->addTerm(new fl::Ramp("LOW", 30.000, 0.000));
	buffer->addTerm(new fl::Triangle("MODERATE", 0.000, 30.000, 100.0));
	buffer->addTerm(new fl::Ramp("HIGH", 30.000, 100.000));
	engine->addInputVariable(buffer);



	// Network Selection FIS Classification
	fl::OutputVariable* nc = new fl::OutputVariable;
	nc->setName("Classification");

	// 0 - 1
//	nc->setRange(0.000, 1.000);
//	nc->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	nc->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	nc->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	nc->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	nc->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 -5
	nc->setRange(0.000, 5.000);
	nc->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	nc->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	nc->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	nc->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	nc->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	nc->setLockOutputValueInRange(true);
	nc->setDefaultValue(fl::nan);
	//nc->setLockValidOutput(true);
	nc->setDefuzzifier(new fl::Centroid(500));
	//nc->output()->setAccumulation(new fl::Maximum);
	nc->fuzzyOutput()->setAccumulation(new fl::Maximum);
	engine->addOutputVariable(nc);

    fl::RuleBlock* block = new fl::RuleBlock();
	block->setName("");
	block->setConjunction(new fl::Minimum);
	block->setDisjunction(new fl::Maximum);
	block->setActivation(new fl::Minimum);

	/*** General classifications ***/

	 block->addRule(fl::Rule::parse("if AP is BAD then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  ( AP is REGULAR and MN is BAD )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  ( AP is REGULAR and MN is REGULAR )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  ( AP is GOOD and MN is BAD )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  ( AP is VERY_GOOD and MN is BAD )  then Classification is BAD", engine));

	/*** Detailed classifications  ***/
	 //1.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is LOW )  and AP is GOOD )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is MODERATE )  and AP is GOOD )  then Classification is GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is HIGH )  and AP is GOOD )  then Classification is GOOD", engine));

	 //2.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is LOW )  and AP is GOOD )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is MODERATE )  and AP is GOOD )  then Classification is GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is HIGH )  and AP is GOOD )  then Classification is GOOD", engine));

	 //3.
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is LOW )  and AP is GOOD )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is MODERATE )  and AP is GOOD )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is HIGH )  and AP is GOOD )  then Classification is REGULAR", engine));

	 //4.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is LOW )  and AP is GOOD )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is MODERATE )  and AP is GOOD )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is HIGH )  and AP is GOOD )  then Classification is GOOD", engine));

	 //5.
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is LOW )  and AP is EXCELLENT )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is MODERATE )  and AP is EXCELLENT )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is HIGH )  and AP is EXCELLENT )  then Classification is GOOD", engine));

	 //6.
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is LOW )  and AP is VERY_GOOD )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is MODERATE )  and AP is VERY_GOOD )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is REGULAR and BUFFER is HIGH )  and AP is VERY_GOOD )  then Classification is REGULAR", engine));

	 //7.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is LOW )  and AP is VERY_GOOD )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is MODERATE )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is HIGH )  and AP is VERY_GOOD )  then Classification is GOOD", engine));

	 //8.
	 block->addRule(fl::Rule::parse("if  (  ( MN is BAD and BUFFER is LOW )  and AP is EXCELLENT )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is BAD and BUFFER is MODERATE )  and AP is EXCELLENT )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is BAD and BUFFER is HIGH )  and AP is EXCELLENT )  then Classification is REGULAR", engine));

	 //9.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is LOW )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is MODERATE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is HIGH )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));

	 //10.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is LOW )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is MODERATE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is HIGH )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));

	 //11.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is LOW )  and AP is EXCELLENT )  then Classification is GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is MODERATE )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is HIGH )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));

	 //12.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is LOW )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is MODERATE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is HIGH )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));

	 //13.
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is LOW )  and AP is REGULAR )  then Classification is BAD", engine));
     block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is MODERATE )  and AP is REGULAR )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is VERY_GOOD and BUFFER is HIGH )  and AP is REGULAR )  then Classification is REGULAR", engine));

	 //14.
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is LOW )  and AP is REGULAR )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is MODERATE )  and AP is REGULAR )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is GOOD and BUFFER is HIGH )  and AP is REGULAR )  then Classification is REGULAR", engine));

	 //15.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is LOW )  and AP is REGULAR )  then Classification is BAD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is MODERATE )  and AP is REGULAR )  then Classification is REGULAR", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is HIGH )  and AP is REGULAR )  then Classification is REGULAR", engine));

	 //16.
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is LOW )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is MODERATE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
	 block->addRule(fl::Rule::parse("if  (  ( MN is EXCELLENT and BUFFER is HIGH )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));


     engine->addRuleBlock(block);

     //engine->configure("Minimum", "Maximum", "Minimum", "Maximum", "Centroid", FL_DIVISIONS);

     ap->setInputValue(apt);
     mn->setInputValue(mbn);
     buffer->setInputValue(bt);


     engine->process();
     nc->defuzzify();
     float out = nc->getOutputValue();

    // float out = nc->defuzzify();


//	std::cout << "AP Classification is: " << ap->fuzzify(apt) << "\n";
//	std::cout << "MN Classification is: " << nc->fuzzify(mbn) << "\n";
//	std::cout << "BUFFER Classification is: " << buffer->fuzzify(bt) << "\n";
//	std::cout << "NS Classification is: " << nc->fuzzify(out) << "\n";


     return out;

}

/**
 * v. 1.03 code
 */

//    flScalar FisNS::FIS_NS(flScalar apt,flScalar mbn,flScalar bt) {
//        FuzzyOperator& op = FuzzyOperator::DefaultFuzzyOperator();
//        FuzzyEngine engine("Network Selection FIS-mamdani", op);
//        engine.hedgeSet().add(new fl::HedgeNot);
//        engine.hedgeSet().add(new fl::HedgeSomewhat);
//        engine.hedgeSet().add(new fl::HedgeVery);
//
//        //FIS_AP output goodness of AP
//        fl::InputLVar* ap = new fl::InputLVar("AP");
//        ap->addTerm(new fl::ShoulderTerm("BAD", 0.0, 0.25,true));
//        ap->addTerm(new fl::TriangularTerm("REGULAR", 0.0, 0.5));
//        ap->addTerm(new fl::TriangularTerm("GOOD", 0.25, 0.75));
//        ap->addTerm(new fl::TriangularTerm("VERY_GOOD", 0.50, 1.0));
//        ap->addTerm(new fl::ShoulderTerm("EXCELLENT", 0.75, 1.0,false));
//        engine.addInputLVar(ap);
//
//        //FIS_MN output goodness of network interface
//        fl::InputLVar* mn = new fl::InputLVar("MN");
//        mn->addTerm(new fl::ShoulderTerm("BAD", 0.0, 0.25,true));
//        mn->addTerm(new fl::TriangularTerm("REGULAR", 0.0, 0.5));
//        mn->addTerm(new fl::TriangularTerm("GOOD", 0.25, 0.75));
//        mn->addTerm(new fl::TriangularTerm("VERY_GOOD", 0.50, 1.0));
//        mn->addTerm(new fl::ShoulderTerm("EXCELLENT", 0.75, 1.0,false));
//        engine.addInputLVar(mn);
//
//    	/** Cached content variable. If value = 0, CACHE = FALSE, value = 1, CACHE = TRUE **/
//        fl::InputLVar* cache = new fl::InputLVar("CACHE");
//        cache->addTerm(new fl::ShoulderTerm("FALSE", 0.0, 0.5, true));
//        cache->addTerm(new fl::ShoulderTerm("TRUE", 0.5, 1.0, false));
//        engine.addInputLVar(cache);
//
//        //Ratio between seeds and leeches in the network
////        fl::InputLVar*  bitt = new fl::InputLVar("BitT");
////        bitt->addTerm(new fl::ShoulderTerm("LOW",0.0, 15.0,true));
////        fl::TriangularTerm *triangular = new fl::TriangularTerm("MODERATE", 0.0, 100.0);
////        triangular->setA(0.0);
////        triangular->setB(15.0);
////        triangular->setC(100.0);
////        bitt->addTerm(triangular);
////        bitt->addTerm(new fl::ShoulderTerm("HIGH",15.0, 100.0,false));
////        engine.addInputLVar(bitt);
//
//        //Network Selection FIS Classification
//        fl::OutputLVar* nc = new fl::OutputLVar("Classification");
//        nc->addTerm(new fl::ShoulderTerm("BAD", 0.0, 0.25,true));
//        nc->addTerm(new fl::TriangularTerm("REGULAR", 0.0, 0.5));
//        nc->addTerm(new fl::TriangularTerm("GOOD", 0.25, 0.75));
//        nc->addTerm(new fl::TriangularTerm("VERY_GOOD", 0.50, 1.0));
//        nc->addTerm(new fl::ShoulderTerm("EXCELLENT", 0.75, 1.0,false));
//        engine.addOutputLVar(nc);
//
//        fl::RuleBlock* block = new fl::RuleBlock();
//
//    	/*** General classifications ***/
//
//		 block->addRule(new fl::MamdaniRule("if AP is BAD then Classification is BAD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  ( AP is REGULAR and MN is BAD )  then Classification is BAD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  ( AP is REGULAR and MN is REGULAR )  then Classification is BAD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  ( AP is GOOD and MN is BAD )  then Classification is BAD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  ( AP is VERY_GOOD and MN is BAD )  then Classification is BAD", engine)); //OK
//
//		/*** Detailed classifications  ***/
//		 //1.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
//
//		 //2.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
//
//		 //3.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and CACHE is TRUE )  and AP is GOOD )  then Classification is REGULAR", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and CACHE is FALSE )  and AP is GOOD )  then Classification is REGULAR", engine)); //OK
//
//		 //4.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is GOOD )  then Classification is GOOD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is GOOD )  then Classification is REGULAR", engine)); //OK
//
//		 //5.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is REGULAR", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is REGULAR", engine)); //OK
//
//		 //6.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is REGULAR", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is REGULAR", engine)); //OK
//
//		 //7.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is GOOD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is GOOD", engine)); //OK
//
//		 //8.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is BAD and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is BAD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is BAD and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is BAD", engine)); //OK
//
//		 //9.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK
//
//		 //10.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine)); //OK
//
//		 //11.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine)); //OK
//
//		 //12.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
//
//		 //13.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is TRUE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
//	     block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and CACHE is FALSE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
//
//		 //14.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is TRUE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and CACHE is FALSE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
//
//		 //15.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is REGULAR )  then Classification is REGULAR", engine)); //OK
//
//		 //16.
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is TRUE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine)); //OK
//		 block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and CACHE is FALSE )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine)); //OK
//
////         block->addRule(new fl::MamdaniRule("if AP is BAD then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  ( AP is REGULAR and MN is BAD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  ( AP is REGULAR and MN is REGULAR )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is HIGH )  and AP is GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is LOW )  and AP is GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is MODERATE )  and AP is GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is HIGH )  and AP is GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is LOW )  and AP is GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is LOW )  and AP is GOOD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is MODERATE )  and AP is GOOD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is HIGH )  and AP is GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is LOW )  and AP is GOOD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is MODERATE )  and AP is GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is LOW )  and AP is VERY_GOOD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  ( AP is VERY_GOOD and MN is BAD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is HIGH )  and AP is GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is MODERATE )  and AP is GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is MODERATE )  and AP is EXCELLENT )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is MODERATE )  and AP is VERY_GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is HIGH )  and AP is VERY_GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is LOW )  and AP is VERY_GOOD )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is MODERATE )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is HIGH )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is BAD and BitT is LOW )  and AP is EXCELLENT )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is BAD and BitT is MODERATE )  and AP is EXCELLENT )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is BAD and BitT is HIGH )  and AP is EXCELLENT )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is LOW )  and AP is EXCELLENT )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is LOW )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is HIGH )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is MODERATE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is HIGH )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is LOW )  and AP is VERY_GOOD )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is MODERATE )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is MODERATE )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is HIGH )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is LOW )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is MODERATE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is HIGH )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is MODERATE )  and AP is REGULAR )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is LOW )  and AP is REGULAR )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is HIGH )  and AP is REGULAR )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is MODERATE )  and AP is REGULAR )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is LOW )  and AP is REGULAR )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  ( AP is GOOD and MN is BAD )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is HIGH )  and AP is REGULAR )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is MODERATE )  and AP is REGULAR )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is LOW )  and AP is REGULAR )  then Classification is BAD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is VERY_GOOD and BitT is HIGH )  and AP is REGULAR )  then Classification is REGULAR", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is HIGH )  and AP is VERY_GOOD )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is MODERATE )  and AP is EXCELLENT )  then Classification is EXCELLENT", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is EXCELLENT and BitT is LOW )  and AP is EXCELLENT )  then Classification is VERY_GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is GOOD and BitT is LOW )  and AP is EXCELLENT )  then Classification is GOOD", engine));
////         block->addRule(new fl::MamdaniRule("if  (  ( MN is REGULAR and BitT is HIGH )  and AP is EXCELLENT )  then Classification is GOOD", engine));
//
//         engine.addRuleBlock(block);
//
//            ap->setInputValue(apt);
//            mn->setInputValue(mbn);
//            //bitt->setInputValue(bt);
//            cache->setInputValue(bt);
//            engine.process();
//            fl::flScalar out = nc->output().defuzzify();
//            (void)out; //Just to avoid warning when building
//           /* FL_LOG("Users=" << ap << " Losses=" << mn << "Load=" << bt);
//            FL_LOG("Users is " << users->fuzzify(ap));
//            FL_LOG("Losses is " << losses->fuzzify(mn));
//            FL_LOG("Load is " << load->fuzzify(bt));
//            FL_LOG("AP classification=" << out);
//            FL_LOG("AP Classification is " << ns->fuzzify(out));*/
//            //printf("%f\n",FisNS::FIS_NS(ap, mn, bt));
////            std::cout << "AP Classification is: " << ap->fuzzify(apt) << "\n";
////            std::cout << "MN Classification is: " << nc->fuzzify(mbn) << "\n";
////            std::cout << "CACHE Classification is: " << cache->fuzzify(bt) << "\n";
////            std::cout << "NS Classification is: " << nc->fuzzify(out) << "\n";
// //           FL_LOG("NS Classification is " << nc->fuzzify(out));
////            std::cout <<"cache: " << cache->fuzzify(1.0) << endl;
////            std::cout << "bt: " << bitt->fuzzify(bt) << endl;
////            std::cout << "debug: " << nc->fuzzify(out) << endl;
////            std::cout << "output: " << out << endl;
//
//
//            return out;
//
//    }



