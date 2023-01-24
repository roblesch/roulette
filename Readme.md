# Roulette

This repository stores code for Spring '23 Directed Research Project "Russian Roulette: From Albedo to EARS", advised by Prof. Ulrich Neumann, USC.

Project proposal: [project-proposal](https://blog.roblesch.page/assets/roblesch_project_proposal.pdf)

## To Build

Project source can be found in `renderer/`. Current builds are tested in Visual Studio 22 with CMake 3.24.

This project has external dependencies. Clone submodules before building.

```
$ git submodule update --init --recursive
```

## References

This project would not be possible without the many resources made publicly available by the rendering community.

https://pbr-book.org/

https://github.com/tunabrain/tungsten

https://github.com/mitsuba-renderer/mitsuba3

https://github.com/Twinklebear/ChameleonRT

https://github.com/RayTracing/raytracing.github.io

Thank you to [Benedikt Bitterli](https://benedikt-bitterli.me/resources/) for making freely available the scene assets and Path Tracing algorithms used in this project. Benedikt's [Tungsten Renderer](https://github.com/tunabrain/tungsten) provides an excellent resource for reference implementations and debugging, and his implementations of geometric primitives and various algorithms are the foundation for the renderer that this project uses to explore techniques in Russian Roulette & Splitting.
