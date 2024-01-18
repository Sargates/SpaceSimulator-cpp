#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform vec3 viewPos;
uniform float fovY;
uniform vec2 screenSize;
uniform mat4 matView;

// Output fragment color
out vec4 finalColor;

#define PI 3.14159265358979323846f
#define DEG2RAD PI/180.f

//! `nmzHash33` and `stars` are from https://www.shadertoy.com/view/XsyGWV
vec3 nmzHash33(vec3 q) {
    uvec3 p = uvec3(ivec3(q));
    p = p*uvec3(374761393U, 1103515245U, 668265263U) + p.zxy + p.yzx;
    p = p.yzx*(p.zxy^(p >> 3U));
    return vec3(p^(p >> 16U))*(1.0/vec3(0xffffffffU));
}

vec3 stars(in vec3 p) {
    vec3 c = vec3(0.);
    float res = screenSize.x*0.8;
    
	for (float i=0.;i<3.;i++) {
        vec3 q = fract(p*(.15*res))-0.5;
        vec3 id = floor(p*(.15*res));
        vec2 rn = nmzHash33(id).xy;
        float c2 = 1.-smoothstep(0.,.9,length(q));
        c2 *= step(rn.x,.0005+i*i*0.001);
        c += c2*(mix(vec3(1.0,0.49,0.1),vec3(0.75,0.9,1.),rn.y)*0.25+0.75);
        p *= 1.4;
    }
    return c*c*.7;
}

void main() {
	// convert fov to radians, mod is ok because trig funcs are cyclic
	float clampedFovInRads = mod(fovY, 360.0f) * DEG2RAD;
	clampedFovInRads = clamp(clampedFovInRads, 0.0001f, PI-0.001f); // Clamp fov
	// tan(pi/2) is undef, nudge away from there
	if (clampedFovInRads == PI) { clampedFovInRads -= 0.01f; }

	float dst = 1.0f/tan(clampedFovInRads/2.0f); // Cotangent, distance of "eye" from screen

	vec3 viewPointLocal = vec3(fragTexCoord-0.5, -dst/2); // negative is because you're facing -z and need to invert it. 1/2 is because UV is on [0,1] and not [-1,1]
	// dst calculation is needed to prevent unrealistic sliding of the stars on the horizon -- stars will stay in the same spot when turning

	viewPointLocal.x *= screenSize.x / screenSize.y; // stretch aspect ratio issue

	vec3 ro = viewPos; // Ray origin (camera pos)
	vec3 rd = normalize((matView * vec4(viewPointLocal, 1)).xyz); // Ray direction, multiplied by the camera's rotation transform

	vec3 bg = stars(rd);
	finalColor = vec4(bg, 1);
}