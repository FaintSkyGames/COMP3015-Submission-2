#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D objTex;
layout (binding = 1) uniform sampler2D crackTex;

uniform mat4 ModelViewMatrix;

uniform struct LightBulbInfo {
    vec4 Position; // Light position in eye coords
    vec3 La; // Ambient light intensity
    vec3 L; // Diffuse and specular light intensity
} pointLight;

uniform struct SpotLightInfo {
    vec3 Position; // Light position in eye coords
    vec3 La; // Ambient light intensity
    vec3 L; // Diffuse and specular light intensity
    vec3 Direction; // Direction of spotlight in cam coords
    float Exponent; // Angular attenuation exponent
    float CutOff; // CutOff angle (between 0 & pi/2)
} spotLight[2];

uniform struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // diffuse reflectivity
    vec3 Ks; // specular reflectivity
    float Shininess; // specular shininess factor
} Material;

vec3 blinnPhong(vec3 position, vec3 n){
    //extract colour for each fragment
    vec3 objTexColor = texture(objTex, TexCoord).rgb; // 1 texture
    vec4 overlapTexColor = texture(crackTex, TexCoord).rgba;
    //vec3 texColor = objTexColor;
    vec3 texColor = mix(objTexColor.rgb, overlapTexColor.rgb, overlapTexColor.a);

    // calculate ambient
    vec3 ambient = texColor * Material.Ka * pointLight.La;

    // calculate diffuse
    vec3 s = normalize(vec3(pointLight.Position) - position);
    float sDotN = dot(s, n);
    vec3 diffuse = Material.Kd * sDotN * texColor;

    // calculate specular here
    vec3 spec = vec3(0.0);

    if(sDotN > 0.0){
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
    }

    //return ambient + diffuse + spec;
    return ambient + pointLight.L * (diffuse + spec);
}

vec3 blinnPhongSpot(int light, vec3 position, vec3 n){
    // calculate ambient
    vec3 ambient = spotLight[light].La * Material.Ka;
    vec3 s = normalize(vec3(spotLight[light].Position) - position);

    float cosAng = dot(-s, normalize(spotLight[light].Direction)); //cosine of angle
    float angle = acos(cosAng); // givves actual angle
    float spotScale = 0.0;

    vec3 diffuse;
    vec3 spec = vec3(0.0);

    if(angle < spotLight[light].CutOff){
        spotScale = pow(cosAng, spotLight[light].Exponent);
        float sDotN = dot(s, n);
        diffuse = Material.Kd * sDotN;

        // calculate specular here
        if(sDotN > 0.0){
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            spec = Material.Ks * pow(max(dot(h,n), 0.0), Material.Shininess);
        }
        return ambient + spotScale * spotLight[light].L * (diffuse + spec);
    }

    return ambient + spotScale * spotLight[light].L * (diffuse + spec);
}


void main()
{
    //vec3 n = normalize(NormalMatrix * Normal); // normal vector
    vec4 p = ModelViewMatrix * vec4(Position, 1.0f); // position

    vec3 camCoords = vec3(p); // direction from the surface to the light

    vec4 color;
    color = vec4(blinnPhong(camCoords, Normal), 1);
    color += vec4(blinnPhongSpot(0, camCoords, Normal),1);
    color += vec4(blinnPhongSpot(1, camCoords, Normal),1);
//
//    for(int i = 0; i < 2; i++)
//        color += vec4(blinnPhongSpot(i, camCoords, Normal),1);
//
    FragColor = color;
}