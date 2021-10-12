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
    std::cout << "B-TWE Version 0.1 alpha, \nwritten by Eric Mandolesi, 2021. \nLicense GPL-2.0\n";
    // *************************************************************
    // INPUT SECTION
    // *************************************************************
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

    CallPut callPut;
    TradeType type;
    (data["callput"]>0.)?callPut=CallPut::Call:callPut=CallPut::Put;
    (data["european"]>0.)?type=TradeType::European:type=TradeType::American;
    Option myopt(data["strike"], static_cast<int>(data["days-to-maturity"]), type, callPut);

    std::cout << "Input option: " << myopt<<"\n";

    // *************************************************************
    // BUILD MODEL SECTION
    // *************************************************************

    BinomialTree model = BinomialTree::build(myenv, myopt);

    // *************************************************************
    // OUTPUT SECTION
    // *************************************************************

    std::cout << "Option fair price at time0 (today): " << model.getPrice() << " USD.\n";
    std::cout << "Delta = " << myUtils::computeDelta(myenv,myopt,model) <<"\n";
    std::cout << "Theta = " << myUtils::computeTheta(myenv,myopt,model) <<"\n";
    std::cout << "Gamma = " << myUtils::computeGamma(myenv,myopt,model) <<"\n";
    std::cout << "Vega  = " << myUtils::computeVega(myenv,myopt,model) <<"\n";
    std::cout << "Rho   = " << myUtils::computeRho(myenv,myopt,model) <<"\n";

    return 0;
}
