# Roulette

This repository stores code for Spring '23 Directed Research Project "Russian Roulette: From Albedo to EARS", advised by Prof. Ulrich Neumann, USC.

Project proposal: [project-proposal](https://blog.roblesch.page/assets/roblesch_project_proposal.pdf)

## To Build

Project source can be found in `renderer/`. Current builds are tested in Visual Studio 22 with CMake 3.24.

This project has external dependencies. Clone submodules before building.

```
$ git submodule update --init --recursive
```

## Blog Posts

Development notes are shared on my blog.

- [Pt. 1 - Project Introduction](https://blog.roblesch.page/blog/2023/01/04/ears-1.html)
- [Pt. 2 - Base Renderer Implementation](https://blog.roblesch.page/blog/2023/01/17/ears-2.html)
- [Pt. 3 - Renderer Debugging and Literature Review of RRS](https://blog.roblesch.page/blog/2023/02/07/ears-3.html)

## References

### Path Guiding

Jiří Vorba, Ondřej Karlík, Martin Šik, Tobias Ritschel, and Jaroslav Křivánek. 2014. On-line learning of parametric mixture models for light transport simulation. ACM Trans. Graph. 33, 4, Article 101 (July 2014), 11 pages. https://doi.org/10.1145/2601097.2601203

Thomas Müller, Markus Gross, and Jan Novák. 2017. Practical Path Guiding for Efficient Light-Transport Simulation. Comput. Graph. Forum 36, 4 (July 2017), 91–100. https://doi.org/10.1111/cgf.13227

### Russian Roulette & Splitting

James Arvo and David Kirk. 1990. Particle transport and image synthesis. In Proceedings of the 17th annual conference on Computer graphics and interactive techniques (SIGGRAPH '90). Association for Computing Machinery, New York, NY, USA, 63–66. https://doi.org/10.1145/97879.97886

James Arvo and David Kirk. 1990. Particle transport and image synthesis. SIGGRAPH Comput. Graph. 24, 4 (Aug. 1990), 63–66. https://doi.org/10.1145/97880.97886

Jiří Vorba and Jaroslav Křivánek. 2016. Adjoint-driven Russian roulette and splitting in light transport simulation. ACM Trans. Graph. 35, 4, Article 42 (July 2016), 11 pages. https://doi.org/10.1145/2897824.2925912

Alexander Rath, Pascal Grittmann, Sebastian Herholz, Philippe Weier, and Philipp Slusallek. 2022. EARS: efficiency-aware russian roulette and splitting. ACM Trans. Graph. 41, 4, Article 81 (July 2022), 14 pages. https://doi.org/10.1145/3528223.3530168

---

This project would not be possible without the many resources made publicly available by the rendering community.

https://pbr-book.org/

https://github.com/tunabrain/tungsten

https://github.com/mitsuba-renderer/mitsuba3

https://github.com/Twinklebear/ChameleonRT

https://github.com/RayTracing/raytracing.github.io

Thank you to [Benedikt Bitterli](https://benedikt-bitterli.me/resources/) for making freely available the scene assets and reference implementations used in this project. Benedikt's [Tungsten Renderer](https://github.com/tunabrain/tungsten) provides an excellent reference for many commonly explored phonomena and is the foundation for this project. His implementation of forward path tracing is reused here directly.

[`Copyright (c) 2014 Benedikt Bitterli <benedikt.bitterli (at) gmail (dot) com>`](https://github.com/tunabrain/tungsten/blob/master/LICENSE.txt)
