# B-TWE

Binary-Tree working example (B-TWE) is a dummy project.
It mocks some functionalities of a binary-tree model to price Stock based derivative contracts, particularly:
* European options (put and call)
* American options (put and call)

The underlying stock can yield dividends both continuously and via event-generation from Poisson distribution with defined metric.

B-TWE doesn't make use of external libraries on its own, but the [catch2](https://github.com/catchorg/Catch2) testing framework has been added to help testing fundamental functionalities and catch bugs.
##What it does
It reads option derivative contract and market conditions from an input file and compute the price of the option and the delta.
It also computes the other Greeks. 
## How it does it
* The price is computed via a [Binary-Tree](https://en.wikipedia.org/wiki/Binomial_options_pricing_model) model.
The time step is assumed to be one day and is hard-coded in the system.
* Delta is computed both via finite-differences and via the formula described in Hull chap. 11.
* The other Greeks are computed via central finite-differences.
* Dividends are paid continuously, the dividend rate is subtracted by the risk-free interest rate in discounting. 
* Event-based dividends are generated via Poisson distribution. Each time an event is generated the Stock pays a dividend equal to 10% of its initial value. 
* <mark>Binary-tree data structure is built upon a vector of vectors with increasing dimension and can be traversed using 2 indices, the lower rank moves across the time dimension, the higher rank moves from the lower stock price to the high ones. This means that the stock prices in the tree are sorted for every time grid node.</mark>
* Any node of the binary tree stores both the value for the option and the value for the underlying. 
## What is tested
Unit testing facilities are added to verify some functionalities of the code. *In particular the numerical correctness of Delta is tested*.
Moreover:
* European call is checked against Black-Scholes solution
* European put is checked against Black-Scholes solution
* American call is checked against Black-Scholes solution (non paying dividend, there are theoretical reasons for which it has to match the European call price)
* It is verified that American put price at time0 is higher than the European put with the same features
* The option object works as expected
* Event-based dividends are applied as expected
## Get the code 
This step requires git. I do assume you know how to get and install it, since this hosting portal is named github.
Run in terminal: 
> git clone https://github.com/oLazy/acadia.git
## Build the binaries
A CMakeLists.txt file is provided and will do the job. 
> mkdir build_b-twe \
cd build_b-twe \
cmake ../acadia \
make
## Usage
An input file *data.txt* is provided and shall be modified to define the problem.
Once the input is finalized, type in terminal
> b-twe data.txt

In order to run the unit testing suite, run 
> tests/run_tests

# TODO
As stated in the beginning, this is an exercise project. 
It is workable but to make it really usable it would require some modifications.
It follows the list of features that should be introduced:
* User-defined time step-length.
* Meaningful event-based dividend size (some poor design decisions have been made for this feature).
* Parallelization of the *simulateUnderlyingDynamics()* method. This can be done trivially via open-MP or CUDA. 
* Extensive testing. Unit testing is rarely enough, never excessive. Really, more test is required even at this preliminary stage. 
* Refactoring. This code has been written in spare time and requires a non-negligible amount of work to be turned in a beautiful piece of code. 

# License
This code, if needed, is licensed under the [GPL-2.0](https://opensource.org/licenses/gpl-2.0.php) license.
