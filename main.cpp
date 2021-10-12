/*
 * Write a program to price an American equity option using a binomial tree.
 * Specs:
 *
 * must be written in c++
 * contains at least one class definition
 * calculate NPV and Delta of the option
 * assume constant volatility, risk-free rate and dividend yield
 *
 * Extras:
 *  - compute more greeks
 *  - verify and test Delta
 */
#include <iostream>
#include "Objects.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

int main(int argc, char* argv[]) {
    if(argc<2)throw std::runtime_error("This program must be called with config file argument.");
    std::map<std::string,double> data;
    // std::ifstream is RAII, i.e. no need to call close
    std::ifstream cFile (argv[1]);
    if (cFile.is_open())
    {
        std::string line;
        while(getline(cFile, line)){
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                       line.end());
            if(line[0] == '#' || line.empty())
                continue;
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            data[name] = stod(value);
        }

    }
    else {
        std::cerr << "Couldn't open config file for reading.\n";
    }
    Environment myenv;
    myenv.riskFreeRate = data["risk-free-rate"];
    myenv.underlyingT0Price = data["S-T0-Price"];
    myenv.volatility = data["volatility"];
    myenv.averageDividendsPerYear = data["average-dividends-per-year"];
    myenv.q=data["continuously-yield-dividend"];
    CallPut callPut;
    TradeType type;
    (data["callput"]>0.)?callPut=CallPut::Call:callPut=CallPut::Put;
    (data["european"]>0.)?type=TradeType::European:type=TradeType::American;
    Option myopt(data["strike"], static_cast<int>(data["days-to-maturity"]), type, callPut);

    std::cout << myopt<<"\n";
    return 0;
    Option option1(60, 365, TradeType::European, CallPut::Call);
    Environment env;
    env.riskFreeRate = 5e-2;
    env.underlyingT0Price = 60;
    env.volatility = 0.10;
    env.averageDividendsPerYear = 0;
    std::vector<int> dividendStructure(option1.getTimeToMaturity());
    BinomialTree model = BinomialTree::build(env, option1, dividendStructure);
    double price = model.getPrice();
    std::cout << "European Call - my NPV - BS price: " << price - 4.08 << "\n";
    Option option2(60, 365, TradeType::European, CallPut::Put);
    Environment env2;
    env2.riskFreeRate = 5e-2;
    env2.underlyingT0Price = 60;
    env2.volatility = 0.10;
    env2.averageDividendsPerYear = 0;
    std::vector<int> dividendStructure2(option2.getTimeToMaturity());
    BinomialTree model2 = BinomialTree::build(env2, option2, dividendStructure2);
    double price2 = model2.getPrice();
    std::cout << "European Put - my NPV - BS price: " << price2 - 1.16 << "\n";
    Option option3(60, 365, TradeType::American, CallPut::Call);
    BinomialTree model3 = BinomialTree::build(env, option3, dividendStructure);
    double price3 = model3.getPrice();
    std::cout << "American Call - my NPV - BS price: " << price3 - 4.08 << "\n";
    Option option4(60, 365, TradeType::American, CallPut::Put);
    BinomialTree model4 = BinomialTree::build(env2, option4, dividendStructure2);
    double price4 = model4.getPrice();
    if(price4>=price2)std::cout << "American Put makes sense\n";

    std::cout << "===========================================\n";
    // compute Delta from model. Two ways
    auto deltaBT{myUtils::computeDelta(model)};
    auto deltaFD{myUtils::computeDelta(env,option1,model)};
    // compute Delta from BS
    auto deltaBS{normalCDF( myUtils::BSd1(env,option1) )};
    std::cout << "E Call: Delta binomial tree - analytical: " << deltaBT - deltaBS << "\n";
    std::cout << "E Call: Delta central differences - analytical: " << deltaFD - deltaBS << "\n";
    deltaBT = myUtils::computeDelta(model2);
    deltaFD = myUtils::computeDelta(env2,option2,model2);
    deltaBS = - normalCDF( - myUtils::BSd1(env,option2) );
    std::cout << "E Put: Delta binomial tree - analytical: " << deltaBT - deltaBS << "\n";
    std::cout << "E Put: Delta central differences - analytical: " << deltaFD - deltaBS << "\n";
    return 0;
    /*
    unsigned long seed=13;
    generator.seed(seed);
    Environment env;
    env.underlyingT0Price=60;
    env.riskFreeRate=0.05;
    env.volatility=0.1;
    env.averageDividendsPerYear=0.;

    Option derivative(60,365,TradeType::European,CallPut::Call);
    auto pricingModel = BinomialTree::build(env,derivative);
    auto delta1 = myUtils::computeDelta(pricingModel);
    std::cout << "trade price = " << pricingModel.getNode(0,0).tradeValue << std::endl;
    std::cout << "Delta  1 = " << delta1 << "\n";
    double deltaAnalytical;
    // compute Delta for a European with no dividends using BS formula
    if(derivative.getCallPut()==CallPut::Call){
        deltaAnalytical = normalCDF(myUtils::BSd1(env, derivative));
    }else{
        deltaAnalytical = normalCDF(myUtils::BSd1(env, derivative)) - 1;
    }
    std::cout << "Delta for a European trade which does not yield dividend is " << deltaAnalytical <<"\n";

    // check by comparison with finite differences.
    double h = 0.01; // 1 USd is the typical sensitivity we are interested in
    auto envM = env;
    envM.underlyingT0Price = env.underlyingT0Price-h;
    auto envP = env;
    envP.underlyingT0Price = env.underlyingT0Price+h;
    auto pricingModelM = BinomialTree::build(envM,derivative,pricingModel.getDividendStructure());
    auto pricingModelP = BinomialTree::build(envP,derivative,pricingModel.getDividendStructure());

    auto deltaT = (pricingModelP.getPrice()-pricingModelM.getPrice())/(2*h);
    std::cout << "Delta test = " << deltaT << "\n";
    std::cout << "log10(abs(Delta test - delta)): " << std::log10(std::abs(deltaT-delta1)) << "\n";
    std::cout << "Delta = " << myUtils::computeDelta(env, derivative, pricingModel) << "\n";
    std::cout << "Theta = " << myUtils::computeTheta(env, derivative, pricingModel) << "\n";
    std::cout << "Gamma = " << myUtils::computeGamma(env, derivative, pricingModel) << "\n";
    std::cout << "Vega  = " << myUtils::computeVega(env, derivative, pricingModel) << "\n";
//    double greekTest =
//    myUtils::computeTheta(env,derivative,pricingModel) + // theta
//    env.riskFreeRate*env.underlyingT0Price*myUtils::computeDelta(env,derivative,pricingModel) + //rSDelta
//    0.5*std::pow(env.volatility,2)*std::pow(env.underlyingT0Price,2)*myUtils::computeGamma(env,derivative,pricingModel) - //1/2sigma^2S^2Gamma
//    env.riskFreeRate*pricingModel.getPrice();//rPi ==> Hull pag. 359
//    std::cout << "greekTest " << greekTest << "\n";


//    for (int i = -11; i<12; i++){
//        auto x = double(i)*0.1;
//        std::cout << x << "; " << normalCDF(x) << "\n";
//    }
    return 0;
*/
     }
