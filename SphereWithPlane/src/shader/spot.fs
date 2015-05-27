#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 ProjTexCoord;

layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D ProjectorTex;

struct LightInfo 
{
    vec3 Intensity;
    vec4 Position;
};
uniform LightInfo Light;

struct SpotLightInfo {
    vec4 Position;   // Position in eye coords
    vec3 Intensity;
    vec3 Direction;  // Direction of the spotlight in eye coords.
    vec3 Up;
    float exponent;  // Angular attenuation exponent
    float alfa;
    float beta;
};
uniform SpotLightInfo Spot;

struct MaterialInfo 
{
    vec3 Kd;                    // Diffuse reflectivity
    vec3 Ks;                    // Specular reflectivity
    vec3 Ka;                    // Ambient reflectivity
    float Shininess;            // Specular shininess factor
};
uniform MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;

float piHalf = 1.57079632679;
float pi = 3.14159265359;

vec3 negyzetSpotLight()
{
    float x = tan(radians(Spot.alfa));
    float y = tan(radians(Spot.beta));

    //ptf = spot point to fragmant point
    vec3 ptf = normalize(vec3( Spot.Position) - Position);
    vec3 spotDir = normalize( Spot.Direction);
    vec3 k = cross(spotDir, -ptf);
    vec3 m = cross(spotDir, k);
    float gamma = acos(dot(spotDir, -ptf));
    vec3 mM =  normalize(m);
   
    vec3 up = Spot.Up;
    vec3 upCrossT = normalize(cross(up, spotDir));

    float vetitettAlfa = acos(dot(mM, upCrossT));
    float vetitettBeta = acos(dot(mM, up));


    if (vetitettAlfa > piHalf)
    {
        vetitettAlfa = pi - vetitettAlfa;
    }

    if (vetitettBeta > piHalf)
    {
        vetitettBeta = pi - vetitettBeta;
    }

    // !!!!
    float vetitettX = cos(vetitettAlfa) * tan(gamma);
    float vetitettY = cos(vetitettBeta) * tan(gamma);

    vec3 ambient = Spot.Intensity * Material.Ka;

    if (vetitettX < x && vetitettY < y)
    {
       float spotFactor = pow( dot(-ptf, spotDir), Spot.exponent );
        vec3 v = normalize(vec3(-Position));
        vec3 h = normalize( v + ptf );

        return
            ambient +
            spotFactor * Spot.Intensity * (
              Material.Kd * max( dot(ptf, Normal), 0.0 ) +
              Material.Ks * pow( max( dot(h,Normal), 0.0 ), Material.Shininess )
           );
    } 
    else 
    {
        return ambient;
    }
}

vec3 ads( )
{
    vec3 s = normalize( vec3(Light.Position) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, Normal );

    return
          Light.Intensity * ( Material.Ka +
          Material.Kd * max( dot(s, Normal), 0.0 ) +
          Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess ) );
}


void main() {
    vec3 Color = vec3(0.0);
    Color = negyzetSpotLight();

    vec4 texColor = texture( Tex1, TexCoord );

    vec4 projTexColor = vec4(0.0);
    if( ProjTexCoord.z > 0.0 )
    {
        projTexColor = textureProj( ProjectorTex, ProjTexCoord );
    }

    FragColor = ( vec4(Color,1.0f) + vec4(ads(), 1.0f) ) + projTexColor * 0.5 ;
}
