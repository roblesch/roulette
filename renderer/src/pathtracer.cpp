#include "pathtracer.h"

#include "sample.h"

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
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point);
    cam.sampleDirection(px, direction);

    Ray ray(point.p, direction.d);

    IntersectionData intersection;
    scene.intersect(ray, intersection);
    if (intersection.material) {
        if (intersection.primitive->emitter) return Vec3f(1.0);
        else return (intersection.Ns + 1.0f).normalized();
    }
}

Vec3f PathTracer::trace(const Scene& scene, const Vec2i& px) {
    Camera cam = scene.camera;
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point);
    cam.sampleDirection(px, direction);

    Ray ray(point.p, direction.d);
    ray.setPrimary();

    IntersectionData intersection;
    bool hit = scene.intersect(ray, intersection);

    //if (intersection.material) {
    //    if (intersection.primitive->emitter) return Vec3f(1.0);
    //    else return (intersection.Ns + 1.0f).normalized();
    //}

    // a311923a352efc0f27ab3f4df46aae2fd037c2d3
    int bounce = 0;
    int maxBounces = 64;

    while (hit && bounce < maxBounces) {
        TangentFrame frame(intersection.Ns);
        bool hitBack = frame.normal.dot(ray.d()) > 0.0f;
        if (hitBack) {
            frame.normal = -frame.normal;
            frame.tangent = -frame.tangent;
        }
        Vec3f wi = frame.toLocal(-ray.d());

        // lambertBsdf::sample
        /*
        if (wi.z <= 0.0f) return false;
        event.wo = cosineHemisphere;
        float pdf = cosineHemispherePdf(wo);
        weight = albedo // 1.0f

        vec3f wo = event.frame.toGlobal(event.wo)

        throughput *= event.weight;
        */

        if (intersection.primitive->emitter) return Vec3f(1.0);
        else return (intersection.Ns + 1.0f).normalized();

        //surfaceEvent = makeLocalScatterEvent;
        /*
        TangentFrame frame = primitive->tangentFrame;
        bool hitBackside = frame.normal.dot(dir) > 0.0f;
        if (hitBackside) {
            frame.normal = -frame.normal;
            frame.tangent = -frame.tangent;
        }
        return SurfaceScatterEvent {
            intersection,
            frame,
            wi = frame.toLocal(-ray.dir()),
            flipFrame
        }
        */
        //handleSurface(surfaceEvent, intersection, bounce, ray, throughput, emission);

        bounce++;
        if (bounce < maxBounces) {
            hit = scene.intersect(ray, intersection);
        }
    }

    // 92c8882193b6baf30d8392e46db7502d18b81453
    /*
    Vec3f pos = cam.eye;
    Vec3f dir = cam.sampleDirection(px);
    Ray ray(pos, dir);
    IntersectionPrimitive intersection;

    int bounce = 0;
    int maxBounces = 64;
    Vec3f emission(0.0f);
    Vec3f throughput(1.0f);
    bool hit = scene.intersect(ray, intersection);

    int DimensionsPerBounce = 10;
    int SensorDimensions = 2;

    while (bounce++ < maxBounces) {
        int dim = bounce * DimensionsPerBounce + SensorDimensions;
        Vec3f p = ray.pos() + ray.dir() * intersection.tfar;
        Vec3f w = -ray.dir();
        Vec3f Ng = intersection.normal.normalized();

        Primitive* prim = intersection.primitive;
        Material* mat = intersection.material.get();

        Vec3f Ns = Ng;

        if (Ng.dot(w) < 0.0f) Ng = -Ng;
        if (Ns.dot(w) < 0.0f) Ns = -Ns;

        TangentFrame frame(Ns);

        //emission += prim->emitter.
    }
    */
    /*
    92c8882193b6baf30d8392e46db7502d18b81453
    ... 

    dir = cam.sampleDir
    intersect ray(dir)
    vec3f throughput(1.0f)
    vec3f emission;
    
    while (bounce < maxBounces)
      dim = bounce * DimensionsPerBounce + SensorDimensions // 10, 2
      // geometric information about intersection
      p = ray.origin + ray.tfar * ray.dir
      w = -ray.dir
      Ng = ray.Ng (non-normalized geometric normal)
      lambda = barycentric u-v coordinates on surface
      Normalize Ng

      triangle = intersected surface
      material = intersected material

      if (intersected material is transparent) {
        send a new ray in the same direction from the other side of the surface
      }

      Ns = surface normal at intersection

      back-face test
      if (Ng.dot(w) < 0.0f) Ng = -Ng
      if (Ns.dot(w) < 0.0f) Ns = -Ns

      TangentSpace frame(Ns)
        normal = Ns
        tangent = (abs(Ns.x) > abs(Ns.y)) ? vec3f(0,1,0) : vec3f(1,0,0)
        bitangent = Ns.cross(tangent).normalized()
        tangent = bitangent.cross(normal)

      emission += material.emission*throughput

      bsdfXi0 = vec3f(sample(dim + 1), sample(dim + 2), sample(dim + 3))
      bsdfXi1 = vec3f(sample(dim + 4), sample(dim + 5), sample(dim + 6))
      bsdfXi2 = vec3f(sample(dim + 7), sample(dim + 8), sample(dim + 9))

      ScatterEvent
        xi = bsdfXi0
        wo = frame.toLocal(w) // vec3f(tangent.dot(w), bitangent.dot(w), normal.dot(w))
        Ng = frame.toLocal(ng) // " " "


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
        wi = cosineHemisphere(xi.xy())
        pdf = cosineHemispherePdf(wi)
        throughput = eval(wo, wi) // wi.z <= 0.0f ? vec3f(0.0f) : color*inv_pi
        switchHemisphere = false
       
      wi = frame.toGlobal(event.wi) // tangent*wi.x + bitangent*wi.y + normal*wi.z 
      
      throughput *= abs(event.wi.z) * event.throughput/event.pdf

      if throughput.max = 0.0f break

      if bounce < maxBounces
        ray(from p in direction wi, tnear = near_zero)
        intersect(ray)
    */

    return Vec3f(0.0f);
}
