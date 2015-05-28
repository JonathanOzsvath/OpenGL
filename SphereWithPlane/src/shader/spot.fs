#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in vec4 ProjTexCoord;
in vec4 ShadowCoord;

layout(binding=0) uniform sampler2D Tex1;
layout(binding=1) uniform sampler2D ProjectorTex;
layout(binding=2) uniform sampler2DShadow ShadowMap;

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

void negyzetSpotLight(out vec3 ambient, out vec3 oDiffSpec)
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


    if (vetitettX < x && vetitettY < y)
    {
        float spotFactor = pow( dot(-ptf, spotDir), Spot.exponent );
        vec3 v = normalize(vec3(-Position));
        vec3 h = normalize( v + ptf );

        ambient = Spot.Intensity * Material.Ka;
        oDiffSpec = 
            spotFactor * Spot.Intensity * (
              Material.Kd * max( dot(ptf, Normal), 0.0 ));
    } 
    else 
    {
        ambient = vec3(0.0);
        oDiffSpec = vec3(0.0);
    }
}

void phongModel( vec3 pos, vec3 norm,out vec3 odiff, out vec3 ospec ) {
    vec3 n = Normal;
    if( !gl_FrontFacing ) n = -n;
    vec3 s = normalize(vec3(Light.Position) - Position);
    vec3 v = normalize(-Position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.Ks *
            pow( max( dot(r,v), 0.0 ), Material.Shininess );

    ospec = spec;
    odiff = diffuse;
}


subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{
    vec3 amb1 = Light.Intensity * Material.Ka;

    vec3 diff, spec;
    phongModel( Position, Normal, diff, spec);
    //vec3 spotLight = negyzetSpotLight();
    vec3 amb = vec3(0.0f), diffSpec = vec3(0.0f);

    float closestDepth = textureProj(ShadowMap, ShadowCoord); 

    vec4 color = texture(Tex1, TexCoord);

    vec4 projTexColor = vec4(0.0);
    if(ProjTexCoord.z > 0.0)
    {
        projTexColor = textureProj( ProjectorTex, ProjTexCoord );
        //FragColor = (vec4(spotLight,1.0) * shadow) * color;
        negyzetSpotLight(amb, diffSpec);
    }
    else
    {
        //FragColor =  vec4(phong, 1.0) * color;
        closestDepth = 1.0f;
    }

    vec3 allAmbient = amb + amb1;
    vec3 allDiff = diffSpec + (diff);
    FragColor = vec4((allAmbient + closestDepth * allDiff), 1.0f) * color 
        + closestDepth * projTexColor * vec4(allDiff, 1.0f);
}

subroutine (RenderPassType)
void recordDepth()
{
}

void main() {
    RenderPass();
}