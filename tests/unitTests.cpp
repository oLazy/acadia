//
// Created by eric on 10/10/2021.
//
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../Objects.h"

TEST_CASE("Options price tests", "[American][European][Call][Put]"){
    SECTION( "Option object functionality" ) {
        Option option(40, 365, TradeType::European, CallPut::Call);
        REQUIRE(option.getStrike() == 40);
        REQUIRE(option.getTimeToMaturity() == 365);
        REQUIRE(option.getCallPut() == CallPut::Call);
        REQUIRE(option.getType() == TradeType::European);
        REQUIRE(option.payout(50) == 10);
        REQUIRE(option.payout(30) == 0);
        REQUIRE(option.payout(40) == 0);
        REQUIRE(option.payout(40.1) - 0.1 < 1e-5);
        REQUIRE(option.payout(39.9) == 0);
    }
    SECTION( "Can price a European Call Option" ) {
        Option option2(60, 365, TradeType::European, CallPut::Call);
        Environment env;
        env.riskFreeRate = 5e-2;
        env.underlyingT0Price = 60;
        env.volatility = 0.10;
        env.averageDividendsPerYear = 0;
        std::vector<int> dividendStructure(option2.getTimeToMaturity());
        BinomialTree model = BinomialTree::build(env, option2, dividendStructure);
        double price = model.getPrice();
        REQUIRE(price-4.08 < 1e-2); // From B-S
    }
    SECTION( "Can price a European Put Option") {
        Option option2(60, 365, TradeType::European, CallPut::Put);
        Environment env;
        env.riskFreeRate = 5e-2;
        env.underlyingT0Price = 60;
        env.volatility = 0.10;
        env.averageDividendsPerYear = 0;
        std::vector<int> dividendStructure(option2.getTimeToMaturity());
        BinomialTree model = BinomialTree::build(env, option2, dividendStructure);
        double price = model.getPrice();
        REQUIRE(price-1.16 < 1e-2); // From B-S
    }

    SECTION( "Can Price an American Call Option"){
        Option option(60, 365, TradeType::American, CallPut::Call);
        Environment env;
        env.riskFreeRate = 5e-2;
        env.underlyingT0Price = 60;
        env.volatility = 0.10;
        env.averageDividendsPerYear = 0;
        std::vector<int> dividendStructure(option.getTimeToMaturity());
        BinomialTree model = BinomialTree::build(env, option, dividendStructure);
        double price = model.getPrice();
        REQUIRE(price-4.08 < 1e-2); // From B-S + theoretical results. The price of an American Call equates the price of a European Call (no dividends)
    }

    SECTION( "Can obtain meaningful result for an American Put Option"){
        Option option1(60, 365, TradeType::American, CallPut::Put);
        Option option2(60, 365, TradeType::European, CallPut::Put);
        Environment env;
        env.riskFreeRate = 5e-2;
        env.underlyingT0Price = 60;
        env.volatility = 0.10;
        env.averageDividendsPerYear = 0;
        std::vector<int> dividendStructure(option1.getTimeToMaturity());
        BinomialTree model1 = BinomialTree::build(env, option1, dividendStructure);
        BinomialTree model2 = BinomialTree::build(env, option2, dividendStructure);
        double price1 = model1.getPrice();
        double price2 = model2.getPrice();
        REQUIRE(price1-price2>0); // Price of an American should be higher than the European equivalent
    }
    SECTION( "Dividends are applied correctly" ){
        Option option(60, 365, TradeType::American, CallPut::Put);
        Environment env;
        env.riskFreeRate = 5e-2;
        env.underlyingT0Price = 60;
        env.volatility = 0.10;
        env.averageDividendsPerYear = 0;
        std::vector<int> dividendStructure(option.getTimeToMaturity());
        dividendStructure[0] = 1;
        BinomialTree model = BinomialTree::build(env, option, dividendStructure);
        double underlyingPriceT1U = env.underlyingT0Price * model.getU() - 0.1*env.underlyingT0Price;
        double underlyingPriceT2UD = env.underlyingT0Price * model.getU() * model.getD() - 0.1*env.underlyingT0Price;
        REQUIRE(std::abs( model.getNode(1,1).underlyingValue - underlyingPriceT1U ) < 1e-5);
        REQUIRE(std::abs( model.getNode(2,1).underlyingValue - underlyingPriceT2UD ) < 1e-5);

    }
}

TEST_CASE("Greek tests", "[Greeks]"){
    SECTION( "Can compute Delta" ){
        Environment env;
        env.riskFreeRate = 1e-2;
        env.underlyingT0Price = 100;
        env.volatility = 0.20;
        env.averageDividendsPerYear = 0;
        Option option(60,3,TradeType::European,CallPut::Call);
        std::vector<int> dividendStructure(option.getTimeToMaturity());
        auto pricingModel = BinomialTree::build(env,option,dividendStructure);
        auto delta = myUtils::computeDelta(pricingModel);
        auto deltaFD = myUtils::computeDelta(env, option, pricingModel);
        double diff{delta-deltaFD};
        double adiff{std::abs(diff)};
        REQUIRE (adiff < 0.01);

    }

    SECTION ( "Delta is correct for European Call" ){
        for (int i = -31; i<32; i++) {
            Environment env;
            env.riskFreeRate = 1e-2;
            env.underlyingT0Price = 100;
            env.volatility = 0.20;
            env.averageDividendsPerYear = 0;
            double strike = 60 + static_cast<double>(i);
            Option option(strike, 365, TradeType::European, CallPut::Call);
            std::vector<int> dividendStructure(option.getTimeToMaturity());
            auto pricingModel = BinomialTree::build(env, option, dividendStructure);
            auto deltaFD = myUtils::computeDelta(env, option, pricingModel);
            auto deltaA = normalCDF(myUtils::BSd1(env, option));
            auto diff = (deltaA - deltaFD)/(deltaA + deltaFD);
            REQUIRE (((std::abs(diff) < 0.05)||(deltaA - deltaFD < 0.01)));
        }
    }


    SECTION ( "Delta is correct for European Put" ){
        for (int i = -31; i<32; i++) {
            Environment env;
            env.riskFreeRate = 5e-2;
            env.underlyingT0Price = 60;
            env.volatility = 0.10;
            env.averageDividendsPerYear = 0;
            double strike = 60 + static_cast<double>(i);
            Option option(strike, 365, TradeType::European, CallPut::Put);
            std::vector<int> dividendStructure(option.getTimeToMaturity());
            auto pricingModel = BinomialTree::build(env, option, dividendStructure);
            auto deltaFD = myUtils::computeDelta(env, option, pricingModel);
            auto deltaA = -normalCDF(-myUtils::BSd1(env, option));
            auto diff = (deltaA - deltaFD) / (deltaA + deltaFD);
            REQUIRE (((std::abs(diff) < 0.05) || ((deltaA - deltaFD) < 0.01)));
        }
    }

    SECTION ( "Delta is correct for American Call"){
        for (int i = -31; i<32; i++) {
            Environment env;
            env.riskFreeRate = 1e-2;
            env.underlyingT0Price = 100;
            env.volatility = 0.20;
            env.averageDividendsPerYear = 0;
            double strike = 60 + static_cast<double>(i);
            Option option(strike, 365, TradeType::American, CallPut::Call);
            std::vector<int> dividendStructure(option.getTimeToMaturity());
            auto pricingModel = BinomialTree::build(env, option, dividendStructure);
            auto deltaFD = myUtils::computeDelta(env, option, pricingModel);
            auto deltaA = normalCDF(myUtils::BSd1(env, option));
            auto diff = (deltaA - deltaFD)/(deltaA + deltaFD);
            REQUIRE (((std::abs(diff) < 0.05)||(deltaA - deltaFD < 0.01)));
        }
    }
    SECTION ("Can compute Theta via FD, and it makes sense"){
        Environment env;
        env.riskFreeRate = 1e-2;
        env.underlyingT0Price = 100;
        env.volatility = 0.20;
        env.averageDividendsPerYear = 0;
        Option option(60,365,TradeType::American,CallPut::Call);
        std::vector<int> dividendStructure(option.getTimeToMaturity());
        auto pricingModel = BinomialTree::build(env,option,dividendStructure);
        auto theta = myUtils::computeTheta(env, option, pricingModel);
        REQUIRE(theta<0.);
    }
}

