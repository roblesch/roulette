#include "pathtracer.h"

Vec3f CameraDebugTracer::trace(const Scene &scene, const Vec2i &px) {
    Camera cam = scene.camera;
    int resx = cam.resx;
    int resy = cam.resy;
    float aspect = cam.aspect;
    return Vec3f(
        -1.0f + 2.0f * (float(px.x()) / float(resx)),
        aspect - 2.0f * aspect * (float(px.y()) / float(resy)),
        1).abs();
}

Vec3f IntersectionDebugTracer::trace(const Scene& scene, const Vec2i& px) {
    Camera cam = scene.camera;
    Ray ray(cam.eye, cam.sampleDirection(px));
    IntersectionPrimitive intersection;
    scene.intersect(ray, intersection);
    if (intersection.material) {
        return intersection.material->debug;
    }
    return Vec3f(0.0f);
}

Vec3f PathTracer::trace(const Scene& scene, const Vec2i& px) {
    Camera cam = scene.camera;
    Vec3f pos = cam.eye;
    Vec3f dir = cam.sampleDirection(px);
    Ray ray(pos, dir);
    IntersectionPrimitive intersection;

    int bounce = 0;
    int maxBounces = 64;
    Vec3f emission(0.0f, 0.0f, 0.0f);
    bool hit = scene.intersect(ray, intersection);
    while (hit && bounce < maxBounces) {
    }

    /*
     commit 92c8882193b6baf30d8392e46db7502d18b81453 (HEAD)
     Author: Benedikt Bitterli <mail@noobody.org>
     Date:   Fri Apr 18 16:22:12 2014 +0200

    Initial commit
    dir = cam.sampleDir
    intersect ray(dir)
    vec3f throughput(1.0f)
    while (bounce < maxBounces)
      p = ray.origin + ray.tfar * ray.dir
      w = -ray.dir
      Ng = ray.Ng (non-normalized normal)
      lambda = barycentric u-v coordinates on surface
      Normalize Ng
      triangle = intersected surface
      material = intersected material
      Ns = surface normal at intersection
      if (Ng.dot(w) < 0.0f) Ng = -Ng
      if (Ns.dot(w) < 0.0f) Ns = -Ns
      TangentSpace frame(Ns)

      emission += material.emission*throughput

      event.xi = bsdfXi0
      event.wo = frame.toLocal(w)
      event.Ng = frame.toLocal(Ng)
      event.wi = cosineHemisphere(event.xi.xy)
      event.pdf = cosineHemispherePdf(event.wi)
      event.throughput = eval(event.wo, event.wi)

      throughput *= material.color
      if (lights)
        emission += throughput * estimateDirect(frame, material.bsdf, event, p, Ns, bsdfXi1, lightXi)

      material.bsdf->sample(event)

      throughput *= abs(event.wi.z) * event.throughput/event.pdf
      


    */

    return Vec3f(0.0f);
}
