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
int main() {

    unsigned long seed=13;
    generator.seed(seed);
    Environment env;
    env.underlyingT0Price=60;
    env.riskFreeRate=0.05;
    env.volatility=0.1;
    env.averageDividendsPerYear=0.;

    Option derivative(60,365,TradeType::European,CallPut::Call);
    auto pricingModel = BinomialTree::build(env,derivative);
    auto delta1 = myUtilClass::computeDelta(pricingModel);
    std::cout << "trade price = " << pricingModel.getNode(0,0).tradeValue << std::endl;
    std::cout << "Delta  1 = " << delta1 << "\n";
    double deltaAnalytical;
    // compute Delta for a European with no dividends using BS formula
    if(derivative.getCallPut()==CallPut::Call){
        deltaAnalytical = normalCDF(myUtilClass::BSd1(env,derivative));
    }else{
        deltaAnalytical = normalCDF(myUtilClass::BSd1(env,derivative))-1;
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
    std::cout << "Delta = " << myUtilClass::computeDelta(env,derivative,pricingModel) << "\n";
    std::cout << "Theta = " << myUtilClass::computeTheta(env,derivative,pricingModel) << "\n";
    std::cout << "Gamma = " << myUtilClass::computeGamma(env,derivative,pricingModel) << "\n";
    std::cout << "Vega  = " << myUtilClass::computeVega(env,derivative,pricingModel) << "\n";
//    double greekTest =
//    myUtilClass::computeTheta(env,derivative,pricingModel) + // theta
//    env.riskFreeRate*env.underlyingT0Price*myUtilClass::computeDelta(env,derivative,pricingModel) + //rSDelta
//    0.5*std::pow(env.volatility,2)*std::pow(env.underlyingT0Price,2)*myUtilClass::computeGamma(env,derivative,pricingModel) - //1/2sigma^2S^2Gamma
//    env.riskFreeRate*pricingModel.getPrice();//rPi ==> Hull pag. 359
//    std::cout << "greekTest " << greekTest << "\n";


//    for (int i = -11; i<12; i++){
//        auto x = double(i)*0.1;
//        std::cout << x << "; " << normalCDF(x) << "\n";
//    }
    return 0;
}
