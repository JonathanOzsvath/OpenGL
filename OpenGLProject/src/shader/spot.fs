#version 430

in vec3 Position;
in vec3 Normal;

struct SpotLightInfo {
    vec4 position;   // Position in eye coords
    vec3 intensity;
    vec3 direction;  // Direction of the spotlight in eye coords.
    float exponent;  // Angular attenuation exponent
    float cutoff;    // Cutoff angle (between 0 and 90)
};
uniform SpotLightInfo Spot[4];

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Ambient reflectivity
uniform vec3 Ks;            // Specular reflectivity
uniform float Shininess;    // Specular shininess factor

layout( location = 0 ) out vec4 FragColor;

vec3 adsWithSpotlight( int spotIndex)
{
    vec3 s = normalize( vec3( Spot[spotIndex].position) - Position );
    vec3 spotDir = normalize( Spot[spotIndex].direction);
    float angle = acos( dot(-s, spotDir) );
    float cutoff = radians( clamp( Spot[spotIndex].cutoff, 0.0, 90.0 ) );
    vec3 ambient = Spot[spotIndex].intensity * Ka;

    if( angle < cutoff ) {
        float spotFactor = pow( dot(-s, spotDir), Spot[spotIndex].exponent );
        vec3 v = normalize(vec3(-Position));
        vec3 h = normalize( v + s );

        return
            ambient +
            spotFactor * Spot[spotIndex].intensity * (
              Kd * max( dot(s, Normal), 0.0 ) +
              Ks * pow( max( dot(h,Normal), 0.0 ), Shininess )
           );
    } else {
        return ambient;
    }
}

void main() {
    vec3 Color =vec3(0.0);
    for( int i = 0; i < 4; i++)
    {
        Color += adsWithSpotlight(i);
    }
    FragColor = vec4(Color, 1.0);
}
