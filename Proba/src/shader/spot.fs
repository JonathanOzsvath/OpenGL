#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding=0) uniform sampler2D Tex1;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

struct MaterialInfo
{
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ka;            // Ambient reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;

vec3 ads( )
{
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, Normal );

    return
        LightIntensity * ( Material.Ka +
          Material.Kd * max( dot(s, Normal), 0.0 ) +
          Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess ) );
}


void main() {
    vec4 texColor = texture( Tex1, TexCoord );

    FragColor = (vec4(ads(),1.0)) * texColor;
}
