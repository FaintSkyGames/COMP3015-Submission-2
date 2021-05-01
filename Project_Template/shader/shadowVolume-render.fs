#version 460

in vec3 GPosition;
in vec3 GNormal;
in vec2 GTexCoord;

uniform vec4 LineColor;
flat in int GIsEdge;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform sampler2D Tex;

uniform vec3 Kd; // Diffuse reflectivity
uniform vec3 Ka; // Ambient reflectivity
uniform vec3 Ks; // Specular reflectivity
uniform float Shininess; // Specular shininess factor

layout (location = 0) out vec4 Ambient;
layout (location = 1) out vec4 DiffSpec;

void shade(){
	vec3 s = normalize(vec3(LightPosition) - GPosition );
	vec3 v = normalize(vec3(-GPosition));
	vec3 r = reflect(-s, GNormal);
	vec4 texColor = texture(Tex, GTexCoord);

	Ambient = vec4(texColor.rgb * LightIntensity * Ka, 1.0);
	DiffSpec = vec4(texColor.rgb * LightIntensity * 
		(Kd * max(dot(s,GNormal), 0.0) +
		Ks * pow(max(dot(r,v), 0.0), Shininess)),
		1.0);
}

void main(){
	shade();
}