#version 460

#define PI 3.14159265

in vec3 GPosition;
in vec3 GNormal;
in vec2 GTexCoord;

uniform vec4 LineColor;
flat in int GIsEdge;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

layout (location = 0) out vec4 Ambient;
layout (location = 1) out vec4 DiffSpec;
layout (location = 2) uniform sampler2D Tex;
layout (location = 3) uniform sampler2D Tex2;
layout (location = 4) uniform sampler2D NoiseTex;

void shade(){
	// Create noise variable
	vec4 noise = texture(NoiseTex, GTexCoord);
	float t = (cos(noise.a * PI) + 1.0) / 2.0;

	// Use noise as alpha of overlaping textures
	vec4 texColor = texture(Tex, GTexCoord);
	vec4 tex2Color = texture(Tex2, GTexCoord);
	vec3 col = mix(texColor.rgb, tex2Color.rgb, t);

	vec3 s = normalize(vec3(LightPosition) - GPosition );
	vec3 v = normalize(vec3(-GPosition));
	vec3 r = reflect(-s, GNormal);

	Ambient = vec4(col.rgb * LightIntensity * Ka, 1.0);
	DiffSpec = vec4(col.rgb * LightIntensity * 
		(Kd * max(dot(s,GNormal), 0.0) +
		Ks * pow(max(dot(r,v), 0.0), Shininess)),
		1.0);
}

void main(){
	shade();
}