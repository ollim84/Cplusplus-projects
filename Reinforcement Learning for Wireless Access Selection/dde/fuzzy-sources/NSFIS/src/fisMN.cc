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
#include "fisMN.h"
//#include "fuzzylite/FuzzyLite.h"
#include <limits>
#include <cstdlib>
//#include "fuzzylite/FunctionTerm.h"
#include <fl/Headers.h>

using namespace fl;

/**
 * v. 3.1 code
 */

float FisMN::FIS_MN(float ss,float ec,float bl)
{

	fl::Engine* engine = new fl::Engine;
	engine->setName("Mobile Node FIS-mamdani");

	//TODO:
//	engine->addHedge(new fl::Not);
//	engine->addHedge(new fl::Somewhat);
//	engine->addHedge(new fl::Very);

    //Signal stregth of the interface to the AP
	fl::InputVariable* signal = new fl::InputVariable;
	signal->setName("Signal");
	signal->setRange(0.000, 100.100);
	signal->addTerm(new fl::Trapezoid("LOW", 0.000, 0.000, 44.000, 54.000));
	signal->addTerm(new fl::Triangle("MODERATE", 44.000, 64.000));
	signal->addTerm(new fl::Trapezoid("HIGH", 54.000, 64.000, 100.100, 100.100));
	engine->addInputVariable(signal);

	//Interface Energy consumption normalized value
	fl::InputVariable* energy = new fl::InputVariable;
	energy->setName("Energy");
	energy->setRange(0.000, 100.000);
	energy->addTerm(new fl::Ramp("LOW", 50.000, 0.000));
	energy->addTerm(new fl::Triangle("MODERATE", 0.000, 100.000));
	energy->addTerm(new fl::Ramp("HIGH", 50.000, 100.000));
	engine->addInputVariable(energy);

	//Battery Level in the MN
	fl::InputVariable* battery = new fl::InputVariable;
	battery->setName("Battery");
	battery->setRange(0.000, 100.000);
	battery->addTerm(new fl::Trapezoid("LOW", 0.000, 0.000, 10.000, 60.000));
	battery->addTerm(new fl::Triangle("MODERATE", 10.000, 60.000, 100.000));
	battery->addTerm(new fl::Ramp("HIGH", 60.000, 100.000));
	engine->addInputVariable(battery);

	//Mobile Node FIS Classification
	fl::OutputVariable* mn = new fl::OutputVariable;
	mn->setName("MN");

	// 0 - 1
//	mn->setRange(0.000, 1.000);
//	mn->addTerm(new fl::Ramp("BAD", 0.250, 0.000));
//	mn->addTerm(new fl::Triangle("REGULAR", 0.000, 0.500));
//	mn->addTerm(new fl::Triangle("GOOD", 0.250, 0.750));
//	mn->addTerm(new fl::Triangle("VERY_GOOD", 0.500, 1.000));
//	mn->addTerm(new fl::Ramp("EXCELLENT", 0.750, 1.000));

	// 0 - 5
	mn->setRange(0.000, 5.000);
	mn->addTerm(new fl::Ramp("BAD", 1.250, 0.000));
	mn->addTerm(new fl::Triangle("REGULAR", 0.000, 2.500));
	mn->addTerm(new fl::Triangle("GOOD", 1.250, 3.750));
	mn->addTerm(new fl::Triangle("VERY_GOOD", 2.500, 5.000));
	mn->addTerm(new fl::Ramp("EXCELLENT", 3.750, 5.000));

	mn->setLockOutputValueInRange(true);
	mn->setDefaultValue(fl::nan);
//    	mn->setLockValidOutput(true);
	mn->setDefuzzifier(new fl::Centroid(500));
//    	mn->output()->setAccumulation(new fl::Maximum);
	mn->fuzzyOutput()->setAccumulation(new fl::Maximum);



	engine->addOutputVariable(mn);

    fl::RuleBlock* block = new fl::RuleBlock();

	block->setName("");
	block->setConjunction(new fl::Minimum);
	block->setDisjunction(new fl::Maximum);
	block->setActivation(new fl::Minimum);
//    	block->setTnorm(new fl::Minimum);
//    	block->setSnorm(new fl::Maximum);
//    	block->setActivation(new fl::Minimum);

	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is HIGH and Battery is HIGH then MN is VERY_GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is LOW and Battery is HIGH then MN is VERY_GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is HIGH and Battery is MODERATE then MN is REGULAR", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is HIGH and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is MODERATE and Battery is HIGH then MN is EXCELLENT", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is MODERATE and Battery is MODERATE then MN is GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is MODERATE and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is LOW and Battery is HIGH then MN is EXCELLENT", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is LOW and Battery is MODERATE then MN is VERY_GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is HIGH and Energy is LOW and Battery is LOW then MN is REGULAR", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is HIGH and Battery is HIGH then MN is GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is HIGH and Battery is MODERATE then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is HIGH and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is MODERATE and Battery is HIGH then MN is VERY_GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is MODERATE and Battery is MODERATE then MN is REGULAR", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is MODERATE and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is LOW and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is LOW and Battery is MODERATE then MN is REGULAR", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is LOW and Battery is HIGH then MN is GOOD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is MODERATE and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is MODERATE and Battery is MODERATE then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is MODERATE and Battery is HIGH then MN is REGULAR", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is HIGH and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is HIGH and Battery is MODERATE then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is LOW and Energy is HIGH and Battery is HIGH then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is LOW and Battery is LOW then MN is BAD", engine));
	block->addRule(fl::Rule::parse("if Signal is MODERATE and Energy is LOW and Battery is MODERATE then MN is GOOD", engine));

    engine->addRuleBlock(block);

    //engine->configure("Minimum", "Maximum", "Minimum", "Maximum", "Centroid", FL_DIVISIONS);

	signal->setInputValue(ss);
	energy->setInputValue(ec);
	battery->setInputValue(bl);
    engine->process();
    mn->defuzzify();

    //float out = mn->defuzzify();
    float out = mn->getOutputValue();

//    std::cout << "Signal is " << signal->fuzzify(ss) << "\n";
//    std::cout << "Energy is " << energy->fuzzify(ec) << "\n";
//    std::cout << "Battery is " << battery->fuzzify(bl) << "\n";
//    std::cout << "MN is " << mn->fuzzify(out) << "\n";

	return out;

}

 /**
  * v. 1.03 code
  */

//   flScalar FisMN::FIS_MN(flScalar ss,flScalar ec,flScalar bl) {
//        FuzzyOperator& op = FuzzyOperator::DefaultFuzzyOperator();
//        FuzzyEngine engine("Mobile Node FIS-mamdani", op);
//        engine.hedgeSet().add(new fl::HedgeNot);
//        engine.hedgeSet().add(new fl::HedgeSomewhat);
//        engine.hedgeSet().add(new fl::HedgeVery);
//
//
//
//        //Signal stregth of the interface to the AP
//        fl::InputLVar* signal = new fl::InputLVar("Signal");
//        //signal->addTerm(new fl::ShoulderTerm("LOW",0.0,20.0,true));
//        //signal->addTerm(new fl::TriangularTerm("MODERATE", 10.0,80.0));
//        //signal->addTerm(new fl::ShoulderTerm("HIGH",50.0, 100.0,false));
//
//        fl::TrapezoidalTerm *trapezoidal1 = new fl::TrapezoidalTerm("LOW", 0.0, 54.0);
//        trapezoidal1->setA(0.0);
//        trapezoidal1->setB(0.0);
//        trapezoidal1->setC(44.0);
//        trapezoidal1->setD(54.0);
//        signal->addTerm(trapezoidal1);
//
//        signal->addTerm(new fl::TriangularTerm("MODERATE", 44.0,64.0));
//
//        fl::TrapezoidalTerm *trapezoidal2 = new fl::TrapezoidalTerm("HIGH", 54.0, 100.10);
//        trapezoidal2->setA(54.0);
//        trapezoidal2->setB(64.0);
//        trapezoidal2->setC(100.10);
//        trapezoidal2->setD(100.10);
//        signal->addTerm(trapezoidal2);
//
//        engine.addInputLVar(signal);
//
//        //Interface Energy consumption normalized value
//        fl::InputLVar* energy = new fl::InputLVar("Energy");
//        //energy->addTerm(new fl::ShoulderTerm("LOW",0.0, 20.0,true));
//        //energy->addTerm(new fl::TriangularTerm("MODERATE", 10.0, 90.0));
//        //energy->addTerm(new fl::ShoulderTerm("HIGH",80.0, 100.0,false));
//
//        energy->addTerm(new fl::ShoulderTerm("LOW",0.0, 50.0,true));
//        energy->addTerm(new fl::TriangularTerm("MODERATE", 0.0, 100.0));
//        energy->addTerm(new fl::ShoulderTerm("HIGH",50.0, 100.0,false));
//
//        engine.addInputLVar(energy);
//
//        //Battery Level in the MN
//        fl::InputLVar* battery = new fl::InputLVar("Battery");
//        //battery->addTerm(new fl::ShoulderTerm("LOW",0.0, 40.0,true));
//        //battery->addTerm(new fl::TriangularTerm("MODERATE",0.0,100.0));
//        //battery->addTerm(new fl::ShoulderTerm("HIGH",40.0, 100.0,false));
//
//        fl::TrapezoidalTerm *trapezoidal3 = new fl::TrapezoidalTerm("LOW", 0.0, 60.10);
//        trapezoidal3->setA(0.0);
//        trapezoidal3->setB(0.0);
//        trapezoidal3->setC(10.0);
//        trapezoidal3->setD(60.0);
//        battery->addTerm(trapezoidal3);
//        //battery->addTerm(new fl::TriangularTerm("MODERATE",0.0,100.0));
//        fl::TriangularTerm *triangular = new fl::TriangularTerm("MODERATE", 10.0, 100.0);
//        triangular->setA(10.0);
//        triangular->setB(60.0);
//        triangular->setC(100.0);
//        battery->addTerm(triangular);
//        battery->addTerm(new fl::ShoulderTerm("HIGH",60.0, 100.0,false));
//
//        engine.addInputLVar(battery);
//
//
//
//        //Mobile Node FIS Classification
//        fl::OutputLVar* mn = new fl::OutputLVar("MN");
//        mn->addTerm(new fl::ShoulderTerm("BAD", 0.0, 0.25,true));
//        mn->addTerm(new fl::TriangularTerm("REGULAR", 0.0, 0.5));
//        mn->addTerm(new fl::TriangularTerm("GOOD", 0.25, 0.75));
//        mn->addTerm(new fl::TriangularTerm("VERY_GOOD", 0.50, 1.0));
//        mn->addTerm(new fl::ShoulderTerm("EXCELLENT", 0.75, 1.0,false));
//        engine.addOutputLVar(mn);
//
//        fl::RuleBlock* block = new fl::RuleBlock();
//
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is HIGH and Battery is HIGH then MN is VERY_GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is LOW and Battery is HIGH then MN is VERY_GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is HIGH and Battery is MODERATE then MN is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is HIGH and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is MODERATE and Battery is HIGH then MN is EXCELLENT", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is MODERATE and Battery is MODERATE then MN is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is MODERATE and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is LOW and Battery is HIGH then MN is EXCELLENT", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is LOW and Battery is MODERATE then MN is VERY_GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is HIGH and Energy is LOW and Battery is LOW then MN is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is HIGH and Battery is HIGH then MN is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is HIGH and Battery is MODERATE then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is HIGH and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is MODERATE and Battery is HIGH then MN is VERY_GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is MODERATE and Battery is MODERATE then MN is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is MODERATE and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is LOW and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is LOW and Battery is MODERATE then MN is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is LOW and Battery is HIGH then MN is GOOD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is MODERATE and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is MODERATE and Battery is MODERATE then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is MODERATE and Battery is HIGH then MN is REGULAR", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is HIGH and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is HIGH and Battery is MODERATE then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is LOW and Energy is HIGH and Battery is HIGH then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is LOW and Battery is LOW then MN is BAD", engine));
//        block->addRule(new fl::MamdaniRule("if Signal is MODERATE and Energy is LOW and Battery is MODERATE then MN is GOOD", engine));
//
//        engine.addRuleBlock(block);
//
//
//            signal->setInput(ss);
//            energy->setInput(ec);
//            battery->setInput(bl);
//            engine.process();
//            fl::flScalar out = mn->output().defuzzify();
//            (void)out; //Just to avoid warning when building
//           /* FL_LOG(" Signal=" << ss << " Energy=" << ec << " Battery=" << bl);
//            FL_LOG("Signal is " << signal->fuzzify(ss));
//            FL_LOG("Energy is " << energy->fuzzify(ec));
//            FL_LOG("Battery is " << battery->fuzzify(bl));
//            FL_LOG("MN Classification=" << out);
//            FL_LOG("MN Classification is " << mn->fuzzify(out));*/
//            return out;
//
//    }


