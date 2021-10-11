//
// Created by eric on 08/10/2021.
//

#ifndef ACADIA_INTERVIEW_OBJECTS_H
#define ACADIA_INTERVIEW_OBJECTS_H

#include <utility>
#include <vector>
#include <cmath>
#include <ostream>
#include <random>

std::default_random_engine generator;

enum class TradeType{
    European,
    American
};
inline std::ostream& operator<<(std::ostream& ostream, TradeType v){
    switch (v) {
        case TradeType::European: return ostream<<"European";
        case TradeType::American: return ostream<<"American";
        default: throw std::invalid_argument("Only European and American Options are supported.");
    }
}
enum class CallPut{
    Call,
    Put
};
inline std::ostream& operator<<(std::ostream& ostream, CallPut v){
    switch (v) {
        case CallPut::Call: return ostream<<"Call";
        case CallPut::Put: return ostream<<"Put";
        default: throw std::invalid_argument("Only Call and Put Options are supported.");
    }
}

double normalCDF(double x){
    return 0.5*erfc(-x * M_SQRT1_2);
}

class Option{
private:
    double strike;
    unsigned daysToMaturity;
    TradeType type;
    CallPut callPut;
public:
    /**
     * Represents an European or American option with the following parameters.
     * This code does not model other kind of contract with optionality.
     * @param strike exercise price
     * @param timeToMaturity time in days before option expires
     * @param type European or American
     * @param callPut Call or Put
     */
    Option(double strike, unsigned timeToMaturity, TradeType type, CallPut callPut) : strike(strike),
                                                                                      daysToMaturity(timeToMaturity),
                                                                                      type(type), callPut(callPut) {}
    Option() = default;
    [[nodiscard]] double payout(double underlyingValue) const {
        switch (callPut) {
            case CallPut::Call:
                return std::max(underlyingValue-strike,0.);
            case CallPut::Put:
                return std::max(strike-underlyingValue,0.);
            default:
                throw std::invalid_argument("Only Call and Put Options are supported.");
        }
    }
    [[nodiscard]] double getStrike() const {
        return strike;
    }
    [[nodiscard]] unsigned getTimeToMaturity() const {
        return daysToMaturity;
    }
    [[nodiscard]] TradeType getType() const {
        return type;
    }
    [[nodiscard]] CallPut getCallPut() const {
        return callPut;
    }
    friend std::ostream &operator<<(std::ostream &os, const Option &option) {
        os << "strike: " << option.strike << " daysToMaturity: " << option.daysToMaturity << " type: " << option.type
           << " callPut: " << option.callPut;
        return os;
    }
};
struct Environment{
    double underlyingT0Price{0};
    double volatility{0};
    double riskFreeRate{0};
    double averageDividendsPerYear{0};
    [[nodiscard]] Environment copy() const {
        Environment res;
        res.underlyingT0Price = this->underlyingT0Price;
        res.volatility = this->volatility;
        res.riskFreeRate = this->riskFreeRate;
        res.averageDividendsPerYear = this->averageDividendsPerYear;
        return res;
    }
};
struct BinomialTreeNode{
    double underlyingValue{0};
    double tradeValue{0};
};

/**
 * Binomial Tree model object.
 */
class BinomialTree{
private:
    std::vector<std::vector<BinomialTreeNode>> tree;
    const unsigned N;
    double u{0}, d{0}, r{0}, dailyRate{0}, t0underVal{0}, sigma{0}, riskNeutralP{0}, averageDividendsPerYear{0};
    Option o;
    std::vector<int> const dividendStructure;
public:
    /**
     * Build a binomial tree model to price financial Option based on stocks. It is grounded on several market assumptions:
     * @dot every step in the binomial tree represents one calendar day.
     * @dot underlying stock has constant volatility.
     * @dot risk-free rate
     * @dot stock may pay dividends with frequency n times per year. Dividends size is 10% of the initial value of the Stock.
     * Dividends are payed according to a Poisson distribution with mean n/365.25. Also No dividends are allowed on day 0.
     * @param e Market environment. It includes vol, rate, stock price at time t0, number of average dividends payed per year.
     * @param o Derivative trade.
     * @return Model object.
     */
    static BinomialTree build(Environment const& e, Option const& o) {
        // Generate dividend structure
        std::poisson_distribution<int> dividendDistribution(e.averageDividendsPerYear/365.25);
        std::vector<int> dividendStructure(0);
        dividendStructure.push_back(0);
        for (int i=1; i<o.getTimeToMaturity()+1; i++){
            int noDividendsToday = dividendDistribution(generator);
            dividendStructure.push_back(noDividendsToday);
            if(noDividendsToday>0){ // TODO put this print to log system
                std::cout<<noDividendsToday << " dividends payed on the " << i << "-th day\n";
            }
        }
        BinomialTree tree(o.getTimeToMaturity(), dividendStructure);
        for (int n=0; n<tree.getN()+1; n++){
            std::vector<BinomialTreeNode> _n(n+1);
            tree.tree.push_back(_n);
        }
        tree.setEnvironment(e);
        tree.setOption(o);
        return tree;
    };
    static BinomialTree build(Environment const& e, Option const& o, std::vector<int> const& dividendStructure) {
        BinomialTree tree(o.getTimeToMaturity(), dividendStructure);
        for (int n=0; n<tree.getN()+1; n++){
            std::vector<BinomialTreeNode> _n(n+1);
            tree.tree.push_back(_n);
        }
        tree.setEnvironment(e);
        tree.setOption(o);
        return tree;
    };
    [[nodiscard]] BinomialTreeNode getNode(unsigned t, unsigned timesUp) const {
        return tree[t][timesUp];
    }
    [[nodiscard]] int getN() const {return N;}
    [[nodiscard]] const std::vector<int> &getDividendStructure() const {
        return dividendStructure;
    }
    [[nodiscard]] double getU() const {
        return u;
    }
    [[nodiscard]] double getD() const {
        return d;
    }
    [[nodiscard]] double getPrice() const {return tree[0][0].tradeValue;}
private:
    explicit BinomialTree(unsigned n, std::vector<int>  ds):
            N(n),
            dividendStructure(std::move(ds)){};
    void setEnvironment(Environment const& e){
        double volatility = e.volatility;
        sigma = volatility;
        //volatility is the annualized volatility
        u = std::exp(volatility * std::sqrt(1. / 365.25)); // every time step is one day in a year
        d = 1/u;
        r = e.riskFreeRate; // yearly risk-free rate
        dailyRate = r/365.25;
        t0underVal = e.underlyingT0Price; // underlying value at time 0. This is in env as is market info.
        riskNeutralP = (std::exp(dailyRate) - d)/(u-d);
        averageDividendsPerYear = e.averageDividendsPerYear;
        simulateUnderlyingDynamics();
    }
    void setOption(Option const& option){
        o=option;
        computeValuesAtMaturity();
        computeValueAtNodes(); //back-substitution
    };
//    void setNode(unsigned t, unsigned timesUp, BinomialTreeNode node){
//        tree[t][timesUp] = node;
//    }
    void simulateUnderlyingDynamics(){
        double dividendSize = t0underVal*0.1;
        std::vector<int> dividendCumSum(dividendStructure.size());
        std::partial_sum(dividendStructure.begin(),dividendStructure.end(),dividendCumSum.begin(),std::plus<int>());
        double payed{0};

        tree[0][0].underlyingValue = t0underVal;
        for (auto i = 1; i < N+1; i++){ // i is time index here
            for (auto j = 0; j < i+1; j++){ // j is the number of times the values moved up
                // this simulation does not depend on the iteration and can be parallelized.
                // OMP and MPI are good candidates. CUDA makes sense only for huge simulations, as the comm time
                // host/device is typically important
                tree[i][j].underlyingValue = std::max(t0underVal*std::pow(u,j)*pow(d,i-j)-static_cast<double>(dividendCumSum[i-1])*dividendSize,0.); // cannot have stocks with negative price
            }
        }
    }
    void computeValuesAtMaturity(){
        for (auto j=N;j!=-1;j--){
            tree[N][j].tradeValue = o.payout(tree[N][j].underlyingValue);
        }
    }
    void computeValueAtNodes(){
        for(auto i = N-1; i!=-1; i--){
            for (auto j=i;j!=-1;j--){
                tree[i][j].tradeValue = std::exp(-dailyRate)*(
                        riskNeutralP*tree[i+1][j+1].tradeValue +
                        (1.-riskNeutralP)*tree[i+1][j].tradeValue);
                if(o.getType()==TradeType::American){
                    double intrinsicValue = o.payout(tree[i][j].underlyingValue);
                    tree[i][j].tradeValue = std::max(tree[i][j].tradeValue, intrinsicValue);
                }
            }
        }
    }
};
/**
 * myUtils implements the program requirements. It makes explicit use of the classes defined so far
 */
namespace myUtils{
    double BSd1(Environment const& env, Option const& opt){
        double d1 = (std::log(env.underlyingT0Price/opt.getStrike()) +
                     (env.riskFreeRate + 0.5*std::pow(env.volatility,2))*
                     opt.getTimeToMaturity()/365.25)/
                    (env.volatility*std::sqrt(opt.getTimeToMaturity()/365.25));
        return d1;
    }

    double computeDelta(BinomialTree const& model){
        // Hull chap. 11
        auto nodeD = model.getNode(1,0);
        auto nodeU = model.getNode(1,1);
        auto S0 = model.getNode(0,0).underlyingValue;

        return(nodeU.tradeValue-nodeD.tradeValue)/(S0* model.getU() - S0*model.getD());
    }
    double computeDelta(Environment const& env, Option const& opt, BinomialTree const& model){
        // compute Delta via central finite-differences
        double h = 0.01; // 1 USd is the typical sensitivity we are interested in
        auto envM = env.copy();
        auto envP = env.copy();
        envM.underlyingT0Price = env.underlyingT0Price-h;
        envP.underlyingT0Price = env.underlyingT0Price+h;
        auto pricingModelM = BinomialTree::build(envM,opt,model.getDividendStructure());
        auto pricingModelP = BinomialTree::build(envP,opt,model.getDividendStructure());
        double priceP=pricingModelP.getPrice();
        double priceM=pricingModelM.getPrice();
        double delta{(priceP-priceM)/(2*h)};
        return delta;
    }
    double computeTheta(Environment const& env, Option const& opt, BinomialTree const& model){
        // compute Theta via central finite-differences
        // NOTE on the signs: time to maturity and tenor have opposite signs.
        // This is the reason that lead the - on the option+ and the + in the option-
        // i.e. to perturb the time amd move it forward, I have to reduce the time to maturity
        Option oP(opt.getStrike(), opt.getTimeToMaturity()-1, opt.getType(), opt.getCallPut());
        Option oM(opt.getStrike(), opt.getTimeToMaturity()+1, opt.getType(), opt.getCallPut());
        auto const& dividendStructure = model.getDividendStructure();
        auto modelP = BinomialTree::build(env,oP,dividendStructure);
        auto modelM = BinomialTree::build(env,oM,dividendStructure);
        return 0.5*(modelP.getPrice() - modelM.getPrice());
    }
    double computeGamma(Environment const& env, Option const& opt, BinomialTree const& model){
        // compute Gamma via central finite-differences
        double h = 0.01; // 1 USd is the typical sensitivity we are interested in
        auto envM = env.copy();
        auto envP = env.copy();
        envM.underlyingT0Price = env.underlyingT0Price-h;
        envP.underlyingT0Price = env.underlyingT0Price+h;
        auto pricingModelM = BinomialTree::build(envM,opt,model.getDividendStructure());
        auto pricingModelP = BinomialTree::build(envP,opt,model.getDividendStructure());
        return (pricingModelP.getPrice()+pricingModelM.getPrice()-(2*model.getPrice()))*pow(h,-2);
    }
    double computeVega(Environment const& env, Option const& opt, BinomialTree const& model){
        // compute Vega via central finite-differences
        double h = 0.0001; // 0.01% yearly volatility
        auto envM = env.copy();
        auto envP = env.copy();
        envM.volatility = env.volatility-h;
        envP.volatility = env.volatility+h;
        auto pricingModelM = BinomialTree::build(envM,opt,model.getDividendStructure());
        auto pricingModelP = BinomialTree::build(envP,opt,model.getDividendStructure());
        return (pricingModelP.getPrice()-pricingModelM.getPrice())/(2*h);
    }
    double computeRho(Environment const& env, Option const& opt, BinomialTree const& model){
        // compute Vega via central finite-differences
        double h = 0.0001; // 0.01% yearly rate
        auto envM = env.copy();
        auto envP = env.copy();
        envM.riskFreeRate = env.riskFreeRate-h;
        envP.riskFreeRate = env.riskFreeRate+h;
        auto pricingModelM = BinomialTree::build(envM,opt,model.getDividendStructure());
        auto pricingModelP = BinomialTree::build(envP,opt,model.getDividendStructure());
        return (pricingModelP.getPrice()-pricingModelM.getPrice())/(2*h);
    }
};

#endif //ACADIA_INTERVIEW_OBJECTS_H
