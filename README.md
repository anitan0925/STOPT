# STOPT
This code implements stochastic optimization methods for linear models, which currently includes

Methods: averaged SGD[1], SVRG[2], SAGA[3], and AMSVRG[4].

Models: L2-regularized multi-class logistic regression.

Note that sparse implementation for SVRG and SVRG is not supported.

For usage, see codes in ./examples/log_reg.

# Example
To build and run an example of logistic regression on MNIST, use the following command with appropriate modifications, which require the compiler with c++11.

```
cd example/log_reg
make compile
python get_mnist.py
make test_sgd
make test_amsvrg
```

# Result
```
./sgd -f ./configures/sgd_opt.json
# TRAINING
[SGD_PARAMETER]
eta: 10000
t0: 100000
order: 1
average_start: 10000
average_f: 1
lambda: 1e-05
biased_f: 1
epoch: 1, loss: 0.4170378021227031
epoch: 2, loss: 0.3695574622243284
epoch: 3, loss: 0.3400663408247810
epoch: 4, loss: 0.3200237013778091
epoch: 5, loss: 0.3056613513111507
epoch: 6, loss: 0.2949346604700380
epoch: 7, loss: 0.2866549447738446
epoch: 8, loss: 0.2800914930004291
epoch: 9, loss: 0.2747735588563641
epoch: 10, loss:  0.2703856642138038
# CLASSIFY
accuracy: 56075/60000(0.9345833333333333)
...
```
```
./amsvrg -f ./configures/amsvrg_opt.json
# TRAINING
[AMSVRG_PARAMETER]
eta: 1
lambda: 1e-05
accuracy: 0.1
restart method: 1
biased_f: 1
epoch: 1, loss: 2.3025850929947036
epoch: 2, loss: 0.4008068644400870
epoch: 3, loss: 0.3704279228629942
epoch: 4, loss: 0.3704279228629942
epoch: 5, loss: 0.2712763542441488
epoch: 6, loss: 0.2627733131048307
epoch: 7, loss: 0.2627733131048307
epoch: 8, loss: 0.2529704295189749
epoch: 9, loss: 0.2502134868674038
epoch: 10, loss:  0.2502134868674038
# CLASSIFY
accuracy: 55932/60000(0.9322000000000000)
...
```

## References
[1] [W. Xu. Towards Optimal One Pass Large Scale Learning with Averaged Stochastic Gradient Descent. arXiv, 2011.](https://arxiv.org/pdf/1107.2490v2.pdf)

[2] [R. Johnson and T. Zhang. Accelerating Stochastic Gradient Descent using Predictive Variance Reduction. NIPS, 2013.](https://papers.nips.cc/paper/4937-accelerating-stochastic-gradient-descent-using-predictive-variance-reduction.pdf)

[3] [A. Defazio, F. Bach, and S. Lacoste-Julien] SAGA: A Fast Incremental Gradient Method with Support for Non-strongly Convex Composite Objectives. NIPS, 2014.](https://papers.nips.cc/paper/5258-saga-a-fast-incremental-gradient-method-with-support-for-non-strongly-convex-composite-objectives.pdf)

[4] [A. Nitanda. Accelerated Stochastic Gradient Descent for Minimizing Finite Sums. AISTATS, 2016.](http://www.jmlr.org/proceedings/papers/v51/nitanda16.pdf)
