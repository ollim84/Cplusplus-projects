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
#include "fisAP.h"
//#include "fuzzylite/FuzzyLite.h"
#include <limits>
#include <cstdlib>
//#include "fuzzylite/FunctionTerm.h"

#include <fl/Headers.h>

using namespace fl;
 /*
  * Version 3.1 Code
  */


float FisAP::FIS_AP(float nu, float lo, float pl) {

//        FuzzyOperator& op = FuzzyOperator::DefaultFuzzyOperator();
//        FuzzyEngine engine("Access Point FIS-mamdani", op);
//        engine.hedgeSet().add(new fl::HedgeNot);
//        engine.hedgeSet().add(new fl::HedgeSomewhat);
//        engine.hedgeSet().add(new fl::HedgeVery);

		// Declaration of the fuzzy engine
    	fl::Engine* engine = new fl::Engine;
    	engine->setName("Access Point FIS-mamdani");

    	//TODO:
//    	engine->addHedge(new fl::Not);
//    	engine->addHedge(new fl::Somewhat);
//    	engine->addHedge(new fl::Very);

    	//Number of clients associated with the AP (WLAN G)
//    	fl::InputVariable* users = new fl::InputVariable;
//    	users->setName("Users");
//    	users->setRange(0.000, 100.000);
//    	users->addTerm(new fl::Trapezoid("LOW", 0.000,0.000,5.000,10.000));
//    	users->addTerm(new fl::Triangle("MODERATE", 5.000, 10.000, 24.000));
//    	users->addTerm(new fl::Trapezoid("HIGH", 10.000,24.000,100.000,100.000));
//    	engine->addInputVariable(users);

    	//Number of clients associated with the AP (WLAN B)
    	fl::InputVariable* users = new fl::InputVariable;
    	users->setName("Users");
    	users->setRange(0.000, 100.000);
    	users->addTerm(new fl::Trapezoid("LOW", 0.000,0.000,2.000,4.000));
    	users->addTerm(new fl::Triangle("MODERATE", 2.000, 4.000, 6.000));
    	users->addTerm(new fl::Trapezoid("HIGH", 4.000,6.000,100.000,100.000));
    	engine->addInputVariable(users);

    	//Ratio between Available Bandwidth and  Maximum Capacity [KBytes/s] of the  AP
    	fl::InputVariable* load = new fl::InputVariable;
    	load->setName("Load");
    	load->setRange(0.000, 1.000);
    	load->addTerm(new fl::Ramp("LOW", 0.300,1.000));
    	load->addTerm(new fl::Triangle("MODERATE", 0.000, 0.300, 1.000));
    	load->addTerm(new fl::Ramp("HIGH", 0.300,0.000));
    	engine->addInputVariable(load);

    	// Percentage of packet losses in the AP
    	fl::InputVariable* losses = new fl::InputVariable;
    	losses->setName("Losses");
    	losses->setRange(0.000, 100.000);
    	losses->addTerm(new fl::Ramp("LOW", 5.000,0.000));
    	losses->addTerm(new fl::Triangle("MODERATE", 0.000, 5.000, 10.000));
    	losses->addTerm(new fl::Trapezoid("HIGH",5.000,10.000,100.000,100.000));
    	engine->addInputVariable(losses);

    	//  Access Point FIS Classification (0 - 1 )
//    	fl::OutputVariable* ap = new fl::OutputVariable;
//    	ap->setName("AP");
//    	ap->setRange(0.000, 1.000);
//    	ap->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//    	ap->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//    	ap->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//    	ap->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//    	ap->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

    	//  Access Point FIS Classification (0 - 5)
    	fl::OutputVariable* ap = new fl::OutputVariable;
    	ap->setName("AP");
    	ap->setRange(0.000, 5.000);
    	ap->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
    	ap->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
    	ap->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
    	ap->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
    	ap->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

    	ap->setLockOutputValueInRange(true);
    	ap->setDefaultValue(fl::nan);
//    	ap->setLockValidOutput(true);
    	ap->setDefuzzifier(new fl::Centroid(500));
//    	ap->output()->setAccumulation(new fl::Maximum);
    	ap->fuzzyOutput()->setAccumulation(new fl::Maximum);

    	engine->addOutputVariable(ap);

    	fl::RuleBlock* block = new fl::RuleBlock;
    	block->setName("");
    	block->setConjunction(new fl::Minimum);
    	block->setDisjunction(new fl::Maximum);
    	block->setActivation(new fl::Minimum);

//    	block->setTnorm(new fl::Minimum);
//    	block->setSnorm(new fl::Maximum);
//    	block->setActivation(new fl::Minimum);

        block->addRule(fl::Rule::parse("if Users is LOW and Load is HIGH and Losses is LOW then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is HIGH and Losses is MODERATE then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is HIGH and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is MODERATE and Losses is LOW then AP is GOOD", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is MODERATE and Losses is MODERATE then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is MODERATE and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is LOW and Losses is LOW then AP is EXCELLENT", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is LOW and Losses is MODERATE then AP is GOOD", engine));
        block->addRule(fl::Rule::parse("if Users is LOW and Load is LOW and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is HIGH and Losses is LOW then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is HIGH and Losses is MODERATE then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is HIGH and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is MODERATE and Losses is LOW then AP is GOOD", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is MODERATE and Losses is MODERATE then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is MODERATE and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is LOW and Losses is LOW then AP is VERY_GOOD", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is LOW and Losses is MODERATE then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is MODERATE and Load is LOW and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is HIGH and Losses is LOW then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is HIGH and Losses is MODERATE then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is HIGH and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is MODERATE and Losses is LOW then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is MODERATE and Losses is MODERATE then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is MODERATE and Losses is HIGH then AP is BAD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is LOW and Losses is LOW then AP is GOOD", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is LOW and Losses is MODERATE then AP is REGULAR", engine));
        block->addRule(fl::Rule::parse("if Users is HIGH and Load is LOW and Losses is HIGH then AP is BAD", engine));

        engine->addRuleBlock(block);

        //engine->configure("Minimum", "Maximum", "Minimum", "Maximum", "Centroid", FL_DIVISIONS);

		users->setInputValue(nu);
		losses->setInputValue(pl);
		load->setInputValue(lo);

        engine->process();
        ap->defuzzify();
        //float out = ap->defuzzify();
        float out = ap->getOutputValue();


//		std::cout <<"Users is: " << users->fuzzify(nu) << "\n";
//		std::cout <<"Load is: " << load->fuzzify(lo) << "\n";
//		std::cout <<"Losses is: " << losses->fuzzify(pl) << "\n";
//		std::cout << "AP is " << ap->fuzzify(out) << "\n";


		return out;

    }


 /*
  * Version 1.03 Code
  */

//    fl::flScalar FisAP::FIS_AP(fl::flScalar nu,fl::flScalar lo,fl::flScalar pl) {
//
//        FuzzyOperator& op = FuzzyOperator::DefaultFuzzyOperator();
//        FuzzyEngine engine("Access Point FIS-mamdani", op);
//        engine.hedgeSet().add(new fl::HedgeNot);
//        engine.hedgeSet().add(new fl::HedgeSomewhat);
//        engine.hedgeSet().add(new fl::HedgeVery);
//
//        //Number of clients associated width the AP
//        fl::InputLVar* users = new fl::InputLVar("Users");
//        fl::TrapezoidalTerm *trapezoidal = new fl::TrapezoidalTerm("LOW", 0.0, 10.0);
//        trapezoidal->setA(0.0);
//        trapezoidal->setB(0.0);
//        trapezoidal->setC(5.0);
//        trapezoidal->setD(10.0);
//        users->addTerm(trapezoidal);
//        fl::TriangularTerm *triangular = new fl::TriangularTerm("MODERATE", 5.0, 24.0);
//        triangular->setA(5.0);
//        triangular->setB(10.0);
//        triangular->setC(24.0);
//        users->addTerm(triangular);
//
//        fl::TrapezoidalTerm *trapezoidal3 = new fl::TrapezoidalTerm("HIGH", 10.0, 100.0);
//        trapezoidal3->setA(10.0);
//        trapezoidal3->setB(24.0);
//        trapezoidal3->setC(100.0);
//        trapezoidal3->setD(100.0);
//        users->addTerm(trapezoidal3);
//        engine.addInputLVar(users);
//        //users->addTerm(new fl::ShoulderTerm("HIGH",10.0, 24.0,false));
//        //engine.addInputLVar(users);
//
//        //Ratio between Available Bandwidth and  Maximum Capacity [KBytes/s] of the  AP
//
//        fl::InputLVar* load = new fl::InputLVar("Load");
//        load->addTerm(new fl::ShoulderTerm("LOW",0.3, 1.0,false));
//        fl::TriangularTerm *triangular2 = new fl::TriangularTerm("MODERATE", 0.0, 1.0);
//        triangular2->setA(0.0);
//        triangular2->setB(0.3);
//        triangular2->setC(1.0);
//        load->addTerm(triangular2);
//        load->addTerm(new fl::ShoulderTerm("HIGH",0.0, 0.3,true));
//        engine.addInputLVar(load);
//
//        //Percentage of packet losses in the AP
//        fl::InputLVar* losses = new fl::InputLVar("Losses");
//        losses->addTerm(new fl::ShoulderTerm("LOW",0.0, 5.0,true));
//        fl::TriangularTerm *triangular3 = new fl::TriangularTerm("MODERATE", 0.0, 10.0);
//        //triangular3 = new fl::TriangularTerm("MODERATE", 0.0, 10.0);
//        triangular3->setA(0.0);
//        triangular3->setB(5.0);
//        triangular3->setC(10.0);
//        losses->addTerm(triangular3);
//        fl::TrapezoidalTerm *trapezoidal2 = new fl::TrapezoidalTerm("HIGH", 0.0, 100.0);
//        trapezoidal2->setA(5.0);
//        trapezoidal2->setB(10.0);
//        trapezoidal2->setC(100.0);
//        trapezoidal2->setD(100.0);
//        losses->addTerm(trapezoidal2);
//        engine.addInputLVar(losses);
//
//        //Access Point FIS Classification
//        fl::OutputLVar* ap= new fl::OutputLVar("AP");
//        ap->addTerm(new fl::ShoulderTerm("BAD", 0.0, 0.25,true));
//        ap->addTerm(new fl::TriangularTerm("REGULAR", 0.0, 0.5));
//        ap->addTerm(new fl::TriangularTerm("GOOD", 0.25, 0.75));
//        ap->addTerm(new fl::TriangularTerm("VERY_GOOD", 0.50, 1.0));
//        ap->addTerm(new fl::ShoulderTerm("EXCELLENT", 0.75, 1.0,false));
//        engine.addOutputLVar(ap);
//
//        fl::RuleBlock* block = new fl::RuleBlock();
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is HIGH and Losses is LOW then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is HIGH and Losses is MODERATE then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is HIGH and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is MODERATE and Losses is LOW then AP is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is MODERATE and Losses is MODERATE then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is MODERATE and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is LOW and Losses is LOW then AP is EXCELLENT", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is LOW and Losses is MODERATE then AP is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is LOW and Load is LOW and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is HIGH and Losses is LOW then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is HIGH and Losses is MODERATE then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is HIGH and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is MODERATE and Losses is LOW then AP is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is MODERATE and Losses is MODERATE then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is MODERATE and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is LOW and Losses is LOW then AP is VERY_GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is LOW and Losses is MODERATE then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is MODERATE and Load is LOW and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is HIGH and Losses is LOW then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is HIGH and Losses is MODERATE then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is HIGH and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is MODERATE and Losses is LOW then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is MODERATE and Losses is MODERATE then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is MODERATE and Losses is HIGH then AP is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is LOW and Losses is LOW then AP is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is LOW and Losses is MODERATE then AP is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Users is HIGH and Load is LOW and Losses is HIGH then AP is BAD", engine));
//
//        engine.addRuleBlock(block);
//
//            users->setInput(nu);
//            losses->setInput(pl);
//            load->setInput(lo);
//            engine.process();
//            fl::flScalar out = ap->output().defuzzify();
//           /* (void)out; //Just to avoid warning when building
//            FL_LOG("Users=" << nu << " Losses=" << pl << "Load=" << lo);
//            FL_LOG("Users is " << users->fuzzify(nu));
//            FL_LOG("Losses is " << losses->fuzzify(pl));
//            FL_LOG("Load is " << load->fuzzify(lo));
//            FL_LOG("AP classification=" << out);
//            FL_LOG("AP Classification is " << ap->fuzzify(out));*/
////            std::cout <<"Users is: " << users->fuzzify(nu) << "\n";
////            std::cout <<"Load is: " << load->fuzzify(lo) << "\n";
////            std::cout <<"Losses is: " << losses->fuzzify(pl) << "\n";
//            return out;
//
//    }



