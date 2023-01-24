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
    PositionSample point;
    DirectionSample direction;

    cam.samplePosition(point);
    cam.sampleDirection(px, direction);

    Ray ray(point.p, direction.d);

    IntersectionData intersection;
    scene.intersect(ray, intersection);
    if (intersection.material) {
        if (intersection.primitive->emissive()) return Vec3f(1.0);
        else return (intersection.Ns + 1.0f).normalized();
    }
    return Vec3f(0.0f);
}

/* === */
int maxBounces = 64;

bool isConsistent(const SurfaceScatterEvent& event, const Vec3f& w) {
    bool geometricBackside = (w.dot(event.data->Ng) < 0.0f);
    bool shadingBackside = (event.wo.z() < 0.0f) ^ event.flippedFrame;
    return geometricBackside == shadingBackside;
}

Vec3f generalizedShadowRay(const Scene& scene, Ray &ray, const shared_ptr<Primitive> light, int bounce) {
    IntersectionData data;
    float tfar_i = ray.tfar();
    Vec3f throughput(1.0f);
    do {
        bool hit = scene.intersect(ray, data) && data.primitive != &(*light);
        if (hit) {
            return Vec3f(0.0f);
        }
        if (data.primitive == nullptr || data.primitive == &(*light))
            return throughput;
        tfar_i -= ray.tfar();
        ray = Ray(ray.p() + ray.d() * ray.tfar(), ray.d());
        ray.tfar(tfar_i);
    } while(true);
    return Vec3f(1.0f);
}

Vec3f attenuatedEmission(const Scene& scene, const shared_ptr<Primitive> light, float expectedDist, IntersectionData& data, int bounce, Ray& ray) {
    float fudgeFactor = 1.0f + 1e-3f;
    /*ray.tfar(expectedDist);*/
    if (!light->intersect(ray, data) || ray.tfar() * fudgeFactor < expectedDist)
        return Vec3f(0.0f);
    data.p = ray.p() + ray.d() * ray.tfar();
    data.w = ray.d();
    light->setIntersectionData(data);

    Vec3f transmittance = generalizedShadowRay(scene, ray, light, bounce); //TODO:: shadowray
    if (transmittance == 0.0f)
        return Vec3f(0.0f);

    return transmittance * light->evalDirect(data);
}

Vec3f lightSample(const Scene& scene, const shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    LightSample sample;
    if (!light->sampleDirect(event.data->p, sample))
        return Vec3f(0.0f);

    event.wo = event.frame.toLocal(sample.d);
    if (!isConsistent(event, sample.d))
        return Vec3f(0.0f);

    Vec3f f = event.data->material->eval(event);
    if (f == 0.0f)
        return Vec3f(0.0f);

    Ray ray = parentRay.scatter(event.data->p, sample.d);
    ray.setPrimary(false);

    IntersectionData data;
    Vec3f e = attenuatedEmission(scene, light, sample.dist, data, bounce, ray);
    if (e == 0.0f)
        return Vec3f(0.0f);

    Vec3f lightF = f * e / sample.pdf;
    lightF *= powerHeuristic(sample.pdf, event.data->material->pdf(event));

    return lightF;
}

Vec3f bsdfSample(const Scene& scene, const shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    if (!event.data->material->sample(event))
        return Vec3f(0.0f);
    if (event.weight == 0.0f)
        return Vec3f(0.0f);

    Vec3f wo = event.frame.toGlobal(event.wo);
    if (!isConsistent(event, wo))
        return Vec3f(0.0f);

    Ray ray = parentRay.scatter(event.data->p, wo);
    ray.setPrimary(false);

    IntersectionData data;
    Vec3f e = attenuatedEmission(scene, light, -1.0f, data, bounce, ray);

    if (e == Vec3f(0.0f))
        return Vec3f(0.0f);

    Vec3f bsdfF = e * event.weight;
    bsdfF *= powerHeuristic(event.pdf, light->directPdf(data, event.data->p));
    return bsdfF;
}

Vec3f sampleDirect(const Scene &scene, const shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
    Vec3f result(0.0f);
    result += lightSample(scene, light, event, bounce, parentRay);
    result += bsdfSample(scene, light, event, bounce, parentRay);
    return result;
}

Vec3f estimateDirect(const Scene &scene, SurfaceScatterEvent& event, int bounce, const Ray &parentRay) {
    float weight = 1.0f;
    const shared_ptr<Primitive> light = scene.lights.at("Light");
    return sampleDirect(scene, light, event, bounce, parentRay) * weight;
}

bool handleSurface(const Scene& scene, SurfaceScatterEvent& event, IntersectionData& data, int bounce, Ray& ray, Vec3f& throughput, Vec3f& emission) {
    Vec3f wo;
    const shared_ptr<Material> bsdf = data.material;

    if (bounce < maxBounces - 1)
        emission += estimateDirect(scene, event, bounce + 1, ray) * throughput;
    if (data.primitive->emissive())
        emission += data.primitive->evalDirect(data) * throughput;

    if (!bsdf->sample(event))
        return false;

    wo = event.frame.toGlobal(event.wo);

    if (!isConsistent(event, wo))
        return false;

    throughput *= event.weight;
    ray.setPrimary(false);

    ray = ray.scatter(ray.p() + ray.d() * ray.tfar(), wo);
    
    return true;
}

Vec3f PathTracer::trace(const Scene& scene, const Vec2i& px) {
    PositionSample point;
    DirectionSample direction;
    scene.camera.samplePosition(point);
    scene.camera.sampleDirection(px, direction);

    Vec3f throughput = point.weight * direction.weight;
    Ray ray(point.p, direction.d);
    ray.setPrimary(true);

    SurfaceScatterEvent surfaceEvent;
    IntersectionData data;
    Vec3f emission(0.0f);

    int bounce = 0;
    bool hit = scene.intersect(ray, data);
    while (hit && bounce < maxBounces) {
        surfaceEvent = SurfaceScatterEvent(data, ray);
        if (!handleSurface(scene, surfaceEvent, data, bounce, ray, throughput, emission))
            break;

        if (throughput.max() == 0.0f)
            break;
        
        float roulettePdf = abs(throughput).max();
        if (bounce > 2 && roulettePdf < 0.1f) {
            if (randf() < roulettePdf)
                throughput /= roulettePdf;
            else
                break;
        }

        if (std::isnan(ray.d().sum() + ray.p().sum()))
            return Vec3f(0.0f);
        if (std::isnan(throughput.sum() + emission.sum()))
            return Vec3f(0.0f);

        bounce++;
        if (bounce < maxBounces)
            hit = scene.intersect(ray, data);
    }
    if (std::isnan(throughput.sum() + emission.sum()))
        return Vec3f(0.0f);
    return emission;
}

//Vec3f lightSample(shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
//    LightSample sample;
//    Vec3f p = event.intersection->p;
//    if (!light->sampleDirect(p, sample))
//        return Vec3f(0.0f);
//
//    event.wo = event.frame.toLocal(sample.d);
//
//    Vec3f albedo = event.intersection->material->albedo;
//
//    // eval bsdf
//    Vec3f f(0.0f);;
//    if (event.wi.z() <= 0.0f || event.wo.z() <= 0.0f)
//        f = Vec3f(0.0f);
//    else
//        f = albedo * INV_PI * event.wo.z();
//    float d = INV_PI;
//    return f;
//    if (f == 0.0f)
//        return Vec3f(0.0f);
//
//    // attenuated emission
//    //Ray ray = parentRay.scatter(event.intersection->p, sample.d, F_NEAR_ZERO);
//    //ray.setPrimary(false);
//    //IntersectionData intersection;
//    //Vec3f e(0.0f);
//    //if (light.shape->intersect(ray, intersection)) {
//    //    intersection.p = ray.p() + ray.d() * ray.tfar();
//    //    intersection.w = ray.d();
//    //    light.shape->setIntersectionData(intersection);
//    //    Vec3f transmittance = generalizedShadowRay();
//    //    if (transmittance != 0.0f)
//    //        e = transmittance * light.evalDirect(data, info);
//    //}
//    Vec3f lightF = f / sample.pdf;
//    lightF *= powerHeuristic(sample.pdf, event.wo.z() * INV_PI);
//    return lightF;
//}
//
//Vec3f bsdfSample(shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
//    event.wo = cosineHemisphere(Vec2f(randf(), randf()));
//    event.pdf = cosineHemispherePdf(event.wo);
//    event.weight = event.intersection->material->albedo;
//
//    Vec3f wo = event.frame.toGlobal(event.wo);
//
//    Vec3f bsdfF = event.weight;
//    //bsdfF *= powerHeuristic(event.pdf, light.directPdf(intersection, event.intersection->p));
//    return bsdfF;
//}
//
//Vec3f sampleDirect(shared_ptr<Primitive> light, SurfaceScatterEvent& event, int bounce, const Ray& parentRay) {
//    Vec3f result(0.0f);
//    result += lightSample(light, event, bounce, parentRay);
//    result += bsdfSample(light, event, bounce, parentRay);
//    return result;
//}

//Vec3f PathTracer::trace(const Scene& scene, const Vec2i& px) {
//    Camera cam = scene.camera;
//    PositionSample point;
//    DirectionSample direction;
//
//    cam.samplePosition(point);
//    cam.sampleDirection(px, direction);
//
//    Ray ray(point.p, direction.d);
//    ray.setPrimary(true);
//
//    IntersectionData intersection;
//    SurfaceScatterEvent surfaceEvent;
//    bool hit = scene.intersect(ray, intersection);
//
//    // a311923a352efc0f27ab3f4df46aae2fd037c2d3
//    int bounce = 0;
//    int maxBounces = 2;
//
//    Vec3f emission(0.0f);
//    Vec3f throughput(1.0f);
//
//    while (hit && bounce < maxBounces) {
//        surfaceEvent = SurfaceScatterEvent(intersection, ray);
//        if (bounce < maxBounces - 1) {
//            emission += sampleDirect(scene.lights.at("Light"), surfaceEvent, bounce, ray) * throughput;
//        }
//
//        surfaceEvent.wo = cosineHemisphere(Vec2f(randf(), randf()));
//        surfaceEvent.pdf = cosineHemispherePdf(surfaceEvent.wo);
//        surfaceEvent.weight = surfaceEvent.intersection->material->albedo;
//        Vec3f wo = surfaceEvent.frame.toGlobal(surfaceEvent.wo);
//        throughput *= surfaceEvent.weight;
//        
//        ray = ray.scatter(ray.p() + ray.d() * ray.tfar(), wo, F_NEAR_ZERO);
//
//        bounce++;
//        if (bounce < maxBounces) {
//            hit = scene.intersect(ray, intersection);
//        }
//    }
//
//    return emission;

    /* handleSurface */
/*
if (bounce < maxBounces - 1) {
    ** estimateDirect**
    * light = only light
    * result = lightSample(light)
    * ** lightsample
    * primitive->quad->sampleDirect(sample)
    * wo = event.frame.toLocal(sample.d)
    * Vec3f f = albedo*INV_PI*event.wo.z();
    * if (f = 0.0f) return 0.0f;
    * ray = parentRay.scatter();
    * ray.setPrimaryRay(false);
    * Vec3f e = attenuatedEmission
    *
}
if (intersection.primitive->emissive()) {
    //emission += info.primitive->evalDirect(data, info)*throughput;
}

** bsdf.sample **
* if wi.z() <= 0.0f break;
* wo = cosineHemisphere(random 2d sample);
* pdf = cosineHemispherePdf(wo); => wo.z()*INV_PI
* weight = material->albedo
* weight *= sqr(eta(event)) // eta is always 1
*

Vec3f wo = frame.toGlobal(wo);
throughput *= weight;
ray = ray.scatter(ray.hitpoint(), wo, epsilon);
*/

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

//    return Vec3f(0.0f);
//}
