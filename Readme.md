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

Development notes are shared [on my blog](https://blog.roblesch.page/blog/2022/11/17/directed-research.html).

## References

### Russian Roulette & Splitting

James Arvo and David Kirk. 1990. Particle transport and image synthesis. In Proceedings of the 17th annual conference on Computer graphics and interactive techniques (SIGGRAPH '90). Association for Computing Machinery, New York, NY, USA, 63–66. https://doi.org/10.1145/97879.97886

Jiří Vorba and Jaroslav Křivánek. 2016. Adjoint-driven Russian roulette and splitting in light transport simulation. ACM Trans. Graph. 35, 4, Article 42 (July 2016), 11 pages. https://doi.org/10.1145/2897824.2925912

Alexander Rath, Pascal Grittmann, Sebastian Herholz, Philippe Weier, and Philipp Slusallek. 2022. EARS: efficiency-aware russian roulette and splitting. ACM Trans. Graph. 41, 4, Article 81 (July 2022), 14 pages. https://doi.org/10.1145/3528223.3530168

---

This project would not be possible without the many resources made publicly available by the rendering community.

https://pbr-book.org/

https://github.com/tunabrain/tungsten

https://github.com/mitsuba-renderer/mitsuba3

Benedikt Bitterli's [Tungsten Renderer](https://github.com/tunabrain/tungsten) is the foundation for this project's implementation of path tracing. His implementations of forward path tracing and geometric primitives are used with some modification.

Alexander Rath's [implementation of ADRRS and EARS](https://github.com/irath96/ears) is used as provided.
