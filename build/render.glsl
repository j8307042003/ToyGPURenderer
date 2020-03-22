
struct Ray {
	vec3 origin;
	vec3 dir;
};


Ray genCamRay(mat4 matrix, vec3 pos, float x, float y, int width, int height) {
	vec3 uv = vec3(x, y, 0);
	// const float film = 0.036;
	// const float lens = 0.05;
	const float film = 0.04;
	const float lens = 0.02;
	vec3 e = vec3(0);
  	vec3 q = vec3(
      			film * (0.5 - uv.x), film / (width / (1.0 * height)) * (uv.y - 0.5), lens
      		 );
  	vec3 q1  = -q;
  	vec3 d   = normalize(q1 - e);
  	Ray ray = Ray((matrix * vec4(e, 1.0)).xyz, (matrix * vec4(d, 0)).xyz);
  	// auto ray = ray3f{transform_point(frame, e), transform_direction(frame, d)};
  	// Ray ray = Ray(pos, d);
  	return ray;
}


float Angle(vec3 a, vec3 b) {
	return acos(dot(normalize(a), normalize(b)));	
}

#define SHAPE_TYPE_SPHERE 1
#define SHAPE_TYPE_TRIANGLE 2
#define SHAPE_TYPE_PLANE 3

struct SphereShape {
	vec4 position;
	float radius;
	// vec3 pad;
};

struct TriangleShape {
	// vec3 vertices[3];
	vec4 v1;
	vec4 v2;
	vec4 v3;
	vec4 normal;
};

struct PlaneShape {
	TriangleShape t1;
	TriangleShape t2;
};


bool solveQuadratic(float a, float b, float c, out highp float x0, out highp float x1) 
{ 
    float discr = b * b - 4 * a * c; 
    if (discr < 0) return false; 
    else if (discr == 0) { 
        x0 = x1 = - 0.5 * b / a; 
    } 
    else { 
        float q = (b > 0) ? 
            -0.5 * (b + sqrt(discr)) : 
            -0.5 * (b - sqrt(discr)); 
        x0 = q / a; 
        x1 = c / q; 
    } 
 
    return true; 
} 




bool RayCastTest_Sphere(SphereShape shape, Ray ray, out vec3 hitPos, out vec3 direction) {
	vec3 oc = ray.origin - shape.position.xyz;
	float b = dot( oc, ray.dir );
	float c = dot( oc, oc ) - shape.radius * shape.radius;
	float h = b*b - c;
	if( h<0.0 ) return false;
	float t = -b - sqrt( h );
	if (t < 0.0) return false;
 	hitPos = ray.dir * t + ray.origin;
	const vec3 normal = normalize(hitPos - shape.position.xyz);
	direction = reflect(ray.dir, normal); 		


	return true;

}

bool RayCastTest_Triangle(TriangleShape shape, Ray ray, out vec3 hitPos, out vec3 direction) {
  //vec3 v1 = shape.v1.xyz;
  //vec3 v2 = shape.v2.xyz;
  //vec3 v3 = shape.v3.xyz;

  vec3 edge1 = shape.v2.xyz - shape.v1.xyz;
  vec3 edge2 = shape.v3.xyz - shape.v1.xyz;


  // compute determinant to solve a linear system
  // vec3 pvec = normalize(cross(ray.dir, edge2));
  vec3 pvec = cross(ray.dir, edge2);
  float det  = dot(edge1, pvec);


  // check determinant and exit if triangle and ray are parallel
  // (could use EPSILONS if desired)
  // if (det == 0) return false;
  float inv_det = 1.0 / det;

  // compute and check first bricentric coordinated
  // vec3 tvec = ray.origin - shape.vertices[0];
  vec3 tvec = ray.origin - shape.v1.xyz;
  float u   = dot(tvec, pvec) * inv_det;

  if (u < 0.0 || u > 1.0) return false;

  // compute and check second bricentric coordinated
  vec3 qvec = cross(tvec, edge1);
  float v   = dot(ray.dir, qvec) * inv_det;
  if (v < 0.0 || (u + v > 1.0)) return false;

  // compute and check ray parameter
  float t = dot(edge2, qvec) * inv_det;
  if (t < 0.05) return false;

  // intersection occurred: set params and exit
  hitPos = ray.origin + t * ray.dir;
  direction = reflect(ray.dir, shape.normal.xyz);
  return true;
}





bool RayCastTest_Plane(PlaneShape shape, Ray ray, out vec3 hitPos, out vec3 direction) {
	return (RayCastTest_Triangle(shape.t1, ray, hitPos, direction)) ||
	       (RayCastTest_Triangle(shape.t2, ray, hitPos, direction));
}

struct Material {
	// vec3 color;
	//float r;
	//float g;
	//float b;
	vec3 color;
	float metalic;
	vec3 emission;
	float roguhness;
	
	float transparency;
	float indexOfRefraction;
	float pad0;
	float pad1;
	//float emission_r;
	//float emission_g;
	//float emission_b;
};


struct ShapeInstance {
	uint shapeType;
	uint shapeIdx;
	uint materialIdx;
};




struct AABB {
	vec4 min;
	vec4 max;
};



bool RayIntersect_AABB(AABB aabb, Ray ray) {
	vec3 invDir = 1.0f / ray.dir;

	float tmin;
	float tmax;

	if ( invDir.x > 0 ){
		tmin = (aabb.min.x - ray.origin.x) * invDir.x;
		tmax = (aabb.max.x - ray.origin.x) * invDir.x;
	}
	else {
		tmin = (aabb.max.x - ray.origin.x) * invDir.x;
		tmax = (aabb.min.x - ray.origin.x) * invDir.x;
	}

	float t0y;
	float t1y;

	if (invDir.y > 0 ){
		t0y = (aabb.min.y - ray.origin.y) * invDir.y;
		t1y = (aabb.max.y - ray.origin.y) * invDir.y;
	}
	else {
		t1y = (aabb.min.y - ray.origin.y) * invDir.y;
		t0y = (aabb.max.y - ray.origin.y) * invDir.y;
	}


	if (tmin > t1y || t0y > tmax) return false;

	tmin = tmin > t0y ? tmin : t0y;
	tmax = tmax < t1y ? tmax : t1y;

	float t0z;
	float t1z;


	if (invDir.z > 0 ){
		t0z = (aabb.min.z - ray.origin.z) * invDir.z;
		t1z = (aabb.max.z - ray.origin.z) * invDir.z;
	}
	else {
		t1z = (aabb.min.z - ray.origin.z) * invDir.z;
		t0z = (aabb.max.z - ray.origin.z) * invDir.z;
	}

	if (tmin > t1z || tmax < t0z) return false;


	if (t0z > tmin) tmin = t0z;
	if (t1z < tmax) tmax = t1z;


	return true;


}


bool RayIntersect_AABB_1(AABB aabb, Ray ray) {
   float t[9];
   t[1] = (aabb.min.x - ray.origin.x)/ ray.dir.x;
   t[2] = (aabb.max.x - ray.origin.x)/ ray.dir.x;
   t[3] = (aabb.min.y - ray.origin.y)/ ray.dir.y;
   t[4] = (aabb.max.y - ray.origin.y)/ ray.dir.y;
   t[5] = (aabb.min.z - ray.origin.z)/ ray.dir.z;
   t[6] = (aabb.max.z - ray.origin.z)/ ray.dir.z;
   t[7] = max(max(min(t[1], t[2]), min(t[3], t[4])), min(t[5], t[6]));
   t[8] = min(min(max(t[1], t[2]), max(t[3], t[4])), max(t[5], t[6]));
   // t[9] = (t[8] < 0 || t[7] > t[8]) ? NOHIT : t[7];
   return (t[8] < 0 || t[7] > t[8]) ? false : true;
   // return t[9];	
}






