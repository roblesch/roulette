## 

Arvo&Kirk - Particle Transport and Image Synthesis

"Linear Boltzmann equation"

Paper adapts various statistical techniques commonly used in neutron transport to stochastic ray tracing/Monte Carlo solutions to the rendering equation

*Russian Roulette* - terminates recursive ray tracing without introducing bias

*Splitting* 

*albedo* is the probability of reimission of an incident particle; typically used to adjust weight [**implicit capture**]

*weight cutoff* - terminate a path when its weight is below a threshold; introduces bias

**Russian Roulette**

*russian roulette* - if weight is below threshold, terminate its history with probability $P$. if it survives, increase its weight by $1/(1-P)$.

$E(W) = P(t)*0 + P(1-t)*\frac{\omega}{1-P(t)} = P*0 + (1-P)*\frac{\omega}{1-P} = \omega$

This elimination of bias does slightly increase variance, but is sufficiently compensated by additional samples which can be collected for the same overall cost.

```{c}
if weight < Thresh then
  sample s uniformly from [0, 1]
  if s < P then terminate path
    else weight <- weight/(1-P)
  end
```

**Splitting**

"If the incident illumination at a surface point can be separated into low-variance strata whose mean values differ greatly, then splitting into one path for each stratum will result in a more efficient estimator".



