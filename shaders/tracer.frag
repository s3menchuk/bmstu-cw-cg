#version 330 core
out vec4 FragColor;

in vec2 PixelCoords;
in vec2 TexCoords;

uniform vec3 CameraPos;
uniform vec3 CameraDir;

uniform float Time;
uniform int FrameNum;
uniform sampler2D AccumTex;

const float PI = 3.14159265359;
const float inf = 1.0 / 0.0;
const float EPSILON = 1e-5;

const int samples_per_pixel = 8;
const int max_ray_bounces = 8;

bool contains(float val, float min_val, float max_val) {
    return min_val <= val && val <= max_val;
}

struct Camera {
    vec3 pos;
    vec3 dir;
    vec3 right;
    vec3 up;
    float near;
    float fov_y;
    float aspect;
};

struct Ray3 {
    vec3 orig;
    vec3 dir;
};

vec3 ray_at(Ray3 ray, float dist) {
    return ray.orig + ray.dir * dist;
}

struct Material {
    vec3 color;
    float metallic;
    vec3 emission_color;
    float emission_strength;
    float alpha;
    float ior;
};

struct HitRecord {
    vec3 point;
    vec3 normal;
    float dist;
    Material material;
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

bool hit(Sphere sphere, Ray3 ray, inout HitRecord hit_record) {
    vec3 oc = ray.orig - sphere.center;
    float dot_value = dot(oc, ray.dir);
    float D = dot_value * dot_value - dot(oc, oc) + sphere.radius * sphere.radius;
    if (D < 0)
        return false;
    float t2 = -dot_value + sqrt(D);
    if (t2 < 0)
        return false;
    float t1 = -dot_value - sqrt(D);
    float t_min = t1 > 0 ? t1 : t2;
    hit_record.dist = t_min;
    hit_record.point = ray_at(ray, t_min);
    hit_record.normal = normalize(hit_record.point - sphere.center);
    hit_record.material = sphere.material;
    return true;
}

struct Quad {
    vec3 q;
    vec3 u, v;
    Material material;
};

bool hit(Quad quad, Ray3 ray, inout HitRecord hit_record) {
    vec3 n = cross(quad.u, quad.v);
    vec3 normal = normalize(n);
    float D = dot(normal, quad.q);
    vec3 w = n / dot(n, n);

    float denom = dot(normal, ray.dir);

    if (abs(denom) < EPSILON)
        return false;

    float t = (D - dot(normal, ray.orig)) / denom;

    if (t < 0)
        return false;

    vec3 intersection = ray_at(ray, t);
    vec3 planar_hitpt_vector = intersection - quad.q;
    float alpha = dot(w, cross(planar_hitpt_vector, quad.v));
    float beta = dot(w, cross(quad.u, planar_hitpt_vector));

    if (!(contains(alpha, 0.0, 1.0) && contains(beta, 0.0, 1.0)))
        return false;

    hit_record.dist = t;
    hit_record.point = intersection;
    hit_record.normal = dot(normal, ray.dir) < 0 ? normal : -normal;
    hit_record.material = quad.material;

    float k = 8.0;
    vec3 v = (hit_record.point - quad.q) * k;
    if (mod(trunc(v.x) + trunc(v.y) + trunc(v.z), 2) == 0)
        hit_record.material.color *= 0.5;
    // hit_record.material.color = 1 - hit_record.material.color;
    return true;
}

Camera camera;

void set_camera(vec3 pos, vec3 dir) {
    camera.pos = pos;
    camera.dir = normalize(dir);

    vec3 world_up = vec3(0.0, 1.0, 0.0);
    camera.right = normalize(cross(camera.dir, world_up));
    camera.up = cross(camera.right, camera.dir);

    camera.near = 1.0;
    camera.fov_y = radians(55.0);
    camera.aspect = 16.0 / 9.0;
}

int COUNT_SPHERES = 0;
Sphere spheres[32];

int COUNT_QUADS = 0;
Quad quads[32];

void create_sphere_scene() {
    Sphere red;
    red.radius = 1.0;
    red.center = vec3(5.0, red.radius, 0.0);
    red.material.color = vec3(1.0, 0.0, 0.0);
    red.material.metallic = 0.0;
    red.material.emission_color = vec3(1.0, 1.0, 1.0);
    red.material.emission_strength = 0.0;
    red.material.alpha = 1.0;

    Sphere green;
    green.radius = 2.0;
    green.center = vec3(-green.radius - 1.0, green.radius, -2.0);
    green.material.color = vec3(0.0, 1.0, 0.0);
    green.material.metallic = 0.25;
    green.material.emission_color = vec3(1.0, 1.0, 1.0);
    green.material.emission_strength = 0.0;
    green.material.alpha = 1.0;

    Sphere blue;
    blue.radius = 3.0;
    blue.center = vec3(2.0, blue.radius, -3.0);
    blue.material.color = vec3(0.0, 0.0, 1.0);
    blue.material.metallic = 0.5;
    blue.material.emission_color = vec3(1.0, 1.0, 1.0);
    blue.material.emission_strength = 0.0;
    blue.material.alpha = 1.0;

    Sphere surface;
    surface.radius = 1000.0;
    surface.center = vec3(0.0, -surface.radius, 0.0);
    surface.material.color = vec3(0.5, 0.5, 0.5);
    surface.material.metallic = 0.0;
    surface.material.emission_color = vec3(1.0, 1.0, 1.0);
    surface.material.emission_strength = 0.0;
    surface.material.alpha = 1.0;

    Sphere sun;
    sun.center = vec3(-50, 10, 50);
    sun.radius = 25.0;
    sun.material.color = vec3(1.0, 1.0, 1.0);
    sun.material.metallic = 0.0;
    sun.material.emission_color = vec3(1.0, 1.0, 1.0);
    sun.material.emission_strength = 1.0;
    sun.material.alpha = 1.0;

    Quad floor_quad;
    floor_quad.q = vec3(-5.0, 0, 5.0);
    floor_quad.u = vec3(0.0, 0.0, -15.0);
    floor_quad.v = vec3(15.0, 0.0, 0.0);
    floor_quad.material.color = vec3(1.0, 1.0, 1.0);
    floor_quad.material.metallic = 0.0;
    floor_quad.material.emission_color = vec3(1.0, 1.0, 1.0);
    floor_quad.material.emission_strength = 0.0;
    floor_quad.material.alpha = 1.0;

    spheres[0] = sun;
    spheres[1] = red;
    spheres[2] = green;
    spheres[3] = blue;
    spheres[4] = surface;

    quads[0] = floor_quad;

    COUNT_SPHERES = 4;
    COUNT_QUADS = 1;

    vec3 pos = vec3(2.0, 3.0, 15.0);
    vec3 dir = vec3(0.0, -0.1, -1.0);
    set_camera(pos, dir);
}

void create_cornell_box() {
    float box_width = 1.5;
    float box_length = 1.5;
    float box_height = 1.5;

    /*
    L - left, R - right
    B - bottom, T - top
    N - near, F - far
    */
    vec3 LBN = vec3(0.0, 0.0, 0.0);
    vec3 LTN = vec3(0.0, box_height, 0.0);
    vec3 LBF = vec3(0.0, 0.0, -box_length);
    vec3 LTF = vec3(0.0, box_height, -box_length);

    vec3 RBN = vec3(box_width, 0.0, 0.0);
    vec3 RTN = vec3(box_width, box_height, 0.0);
    vec3 RBF = vec3(box_width, 0.0, -box_length);
    vec3 RTF = vec3(box_width, box_height, -box_length);

    // Left Wall
    Quad left_wall;
    left_wall.q = LBN;
    left_wall.u = LTN - LBN;
    left_wall.v = LBF - LBN;
    left_wall.material.color = vec3(1.0, 0.0, 0.0);
    left_wall.material.metallic = 0.0;
    left_wall.material.emission_color = vec3(1.0, 1.0, 1.0);
    left_wall.material.emission_strength = 0.0;
    left_wall.material.alpha = 1.0;

    // Right Wall
    Quad right_wall;
    right_wall.q = RBN;
    right_wall.u = RTN - RBN;
    right_wall.v = RBF - RBN;
    right_wall.material.color = vec3(0.0, 1.0, 0.0);
    right_wall.material.metallic = 0.0;
    right_wall.material.emission_color = vec3(1.0, 1.0, 1.0);
    right_wall.material.emission_strength = 0.0;
    right_wall.material.alpha = 1.0;

    // Back Wall
    Quad back_wall;
    back_wall.q = LBF;
    back_wall.u = LTF - LBF;
    back_wall.v = RBF - LBF;
    back_wall.material.color = vec3(0.0, 0.0, 1.0);
    back_wall.material.metallic = 0.0;
    back_wall.material.emission_color = vec3(1.0, 1.0, 1.0);
    back_wall.material.emission_strength = 0.0;
    back_wall.material.alpha = 1.0;

    // Floor
    Quad floor_wall;
    floor_wall.q = LBN;
    floor_wall.u = LBF - LBN;
    floor_wall.v = RBN - LBN;
    floor_wall.material.color = vec3(1.0, 1.0, 1.0);
    floor_wall.material.metallic = 0.0;
    floor_wall.material.emission_color = vec3(1.0, 1.0, 1.0);
    floor_wall.material.emission_strength = 0.0;
    floor_wall.material.alpha = 1.0;

    // Ceiling
    Quad ceiling_wall;
    ceiling_wall.q = LTN;
    ceiling_wall.u = LTF - LTN;
    ceiling_wall.v = RTN - LTN;
    ceiling_wall.material.color = vec3(1.0, 1.0, 1.0);
    ceiling_wall.material.metallic = 0.0;
    ceiling_wall.material.emission_color = vec3(1.0, 1.0, 1.0);
    ceiling_wall.material.emission_strength = 0.0;
    ceiling_wall.material.alpha = 1.0;

    // Front Wall
    // Quad front_wall;
    // front_wall.q = LBN;
    // front_wall.u = LTN - LBN;
    // front_wall.v = RBN - LBN;
    // front_wall.material.color = vec3(1.0, 1.0, 1.0);
    // front_wall.material.metallic = 0.0;
    // front_wall.material.emission_color = vec3(1.0, 1.0, 1.0);
    // front_wall.material.emission_strength = 0.0;
    // front_wall.material.alpha = 1.0;

    // Sphere
    Sphere sphere;
    sphere.radius = 0.3;
    sphere.center = vec3(box_width / 2, sphere.radius, -box_length / 2);
    sphere.material.color = vec3(1.0, 1.0, 1.0);
    sphere.material.metallic = 0;
    sphere.material.emission_color = vec3(1.0, 1.0, 1.0);
    sphere.material.emission_strength = 0.0;
    sphere.material.alpha = 0.05;
    sphere.material.ior = 1.5;

    // Light source
    Quad light;
    light.q = LTN * (1 - EPSILON) + (RTF - LTN) * 0.4;
    light.u = (LTF - LTN) * 0.2;
    light.v = (RTN - LTN) * 0.2;
    light.material.color = vec3(1.0, 1.0, 1.0);
    light.material.metallic = 0.0;
    light.material.emission_color = vec3(1.0, 1.0, 1.0);
    light.material.emission_strength = 5.0;
    light.material.alpha = 1.0;

    quads[0] = left_wall;
    quads[1] = right_wall;
    quads[2] = back_wall;
    quads[3] = floor_wall;
    quads[4] = ceiling_wall;
    quads[5] = light;
    // quads[6] = front_wall;

    spheres[0] = sphere;

    COUNT_QUADS = 6;
    COUNT_SPHERES = 1;

    vec3 pos = vec3(box_width / 2, box_height / 2, 1.0);
    vec3 dir = vec3(0.0, 0, -1.0);
    set_camera(pos, dir);
}

uint hash(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    x ^= x >> 16;
    return x;
}

float rand(inout uint seed) {
    seed = hash(seed);
    return float(seed) / 4294967296.0;  // 2^32
}

void make_basis(vec3 n, out vec3 t, out vec3 b) {
    if (abs(n.z) < 0.999) {
        t = normalize(cross(n, vec3(0.0, 0.0, 1.0)));
    } else {
        t = normalize(cross(n, vec3(0.0, 1.0, 0.0)));
    }
    b = cross(n, t);
}

vec3 random_cosine_hemisphere(vec3 normal, inout uint seed) {
    float u1 = rand(seed);
    float u2 = rand(seed);

    float r = sqrt(u1);
    float phi = 2.0 * PI * u2;

    vec3 localDir = vec3(r * cos(phi), r * sin(phi), sqrt(1.0 - u1));

    vec3 t, b;
    make_basis(normal, t, b);

    return normalize(localDir.x * t + localDir.y * b + localDir.z * normal);
}

bool find_closest_sphere(inout HitRecord closest_hit, Ray3 ray) {
    HitRecord closest, current;
    closest.dist = inf;
    for (int i = 0; i < COUNT_SPHERES; ++i) {
        if (hit(spheres[i], ray, current) && current.dist < closest.dist) {
            closest = current;
        }
    }
    if (isinf(closest.dist)) {
        return false;
    }
    closest_hit = closest;
    return true;
}

bool find_closest_quad(inout HitRecord closest_hit, Ray3 ray) {
    HitRecord closest, current;
    closest.dist = inf;
    for (int i = 0; i < COUNT_QUADS; ++i) {
        if (hit(quads[i], ray, current) && current.dist < closest.dist) {
            closest = current;
        }
    }
    if (isinf(closest.dist)) {
        return false;
    }
    closest_hit = closest;
    return true;
}

bool find_closest_hit(inout HitRecord closest_hit, Ray3 ray) {
    HitRecord current, closest;
    closest.dist = inf;
    if (find_closest_sphere(current, ray) && current.dist < closest.dist)
        closest = current;
    if (find_closest_quad(current, ray) && current.dist < closest.dist)
        closest = current;

    if (isinf(closest.dist))
        return false;
    closest_hit = closest;
    return true;
}

vec3 trace_ray(Ray3 ray, int depth, inout uint seed) {
    vec3 final_light = vec3(0.0, 0.0, 0.0);
    vec3 ray_color = vec3(1.0, 1.0, 1.0);
    bool inside = false;
    for (int i = 0; i < depth; ++i) {
        HitRecord closest_hit;
        if (find_closest_hit(closest_hit, ray)) {
            vec3 emitted_light = closest_hit.material.emission_color * closest_hit.material.emission_strength;
            final_light += emitted_light * ray_color;

            if (rand(seed) < closest_hit.material.alpha) {
                ray.orig = closest_hit.point + closest_hit.normal * EPSILON;
                if (rand(seed) > closest_hit.material.metallic) {
                    ray.dir = random_cosine_hemisphere(closest_hit.normal, seed);
                    ray_color *= closest_hit.material.color;
                } else {
                    ray.dir = reflect(ray.dir, closest_hit.normal);
                }
            } else {
                vec3 N;
                float eta;
                if (inside) {
                    N = -closest_hit.normal;
                    eta = closest_hit.material.ior / 1.0;
                } else {
                    N = closest_hit.normal;
                    eta = 1.0 / closest_hit.material.ior;
                }
                inside = !inside;
                ray.orig = closest_hit.point - N * EPSILON;
                ray.dir = refract(ray.dir, N, eta);
            }

        } else {
            // return vec3(0.0, 0.0, 0.25 + (ray.dir.y + 1) / 4);
            break;
        }
    }
    return final_light;
}

vec3 render() {
    float view_height = 2.0 * tan(camera.fov_y / 2.0) * camera.near;
    float view_width = view_height * camera.aspect;

    float ndc_x = PixelCoords.x;
    float dx = ndc_x * view_width / 2.0;
    float ndc_y = PixelCoords.y;
    float dy = ndc_y * view_height / 2.0;

    Ray3 ray;
    ray.orig = camera.pos;
    ray.dir = normalize(dx * camera.right + dy * camera.up + camera.dir);

    uint seed = uint(gl_FragCoord.x) * 1973u + uint(gl_FragCoord.y) * 9277u + uint(FrameNum) * 26699u;  // PixelCoords for intersting effect

    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < samples_per_pixel; ++i) {
        color += trace_ray(ray, max_ray_bounces, seed);
    }
    color /= samples_per_pixel;

    return color;
}

void init_scene() {
    create_sphere_scene();
    // create_cornell_box();
}

void main() {
    init_scene();
    set_camera(CameraPos, CameraDir);

    vec3 prev_color = texture(AccumTex, TexCoords).rgb;
    vec3 curr_color = render();
    vec3 final_color = (prev_color * float(FrameNum - 1) + curr_color) / float(FrameNum);
    FragColor = vec4(final_color, 1.0);
}