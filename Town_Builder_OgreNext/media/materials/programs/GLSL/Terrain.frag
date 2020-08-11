#version 120

uniform sampler2D covMap0;
uniform sampler2D covMap1;
uniform sampler2D covMap2;
uniform sampler2D splatMap0;
uniform sampler2D splatMap1;
uniform sampler2D splatMap2;
uniform sampler2D splatMap3;
uniform sampler2D splatMap4;
uniform sampler2D splatMap5;
uniform sampler2D splatMap6;
uniform sampler2D splatMap7;
uniform sampler2D splatMap8;
uniform sampler2D splatMap9;
uniform sampler2D splatMap10;
uniform sampler2D splatMap11;
uniform sampler2D normalMap;

uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform vec4 lightPosition;
uniform vec3 eyePosition;
uniform vec4 attenuation;
uniform vec4 ambient;

varying vec2 oUV0;
varying vec4 pos;


vec4 Lit(float NdotL, float NdotH, float m) {

  float amb = 1.0;
  float diffuse = max(NdotL, 0.0);
  float specular = step(0.0, NdotL) * max(NdotH * m, 0.0);

  return vec4(amb, diffuse, specular, 1.0);
}

void main ()
{
	float exponent = 10;

	// Scaling UV map to get texture tiling effect
	vec2 scaledUV = oUV0 * 16;

	// Sampling coverage map Textures				  
	vec4 cov0 = texture2D(covMap0, oUV0).rgba;
	vec4 cov1 = texture2D(covMap1, oUV0).rgba;
	vec4 cov2 = texture2D(covMap2, oUV0).rgba;

	// Sampling splat Textures				  
	vec3 t0  = texture2D(splatMap0,  scaledUV).rgb;
	vec3 t1  = texture2D(splatMap1,  scaledUV).rgb;
	vec3 t2  = texture2D(splatMap2,  scaledUV).rgb;
	vec3 t3  = texture2D(splatMap3,  scaledUV).rgb;
	vec3 t4  = texture2D(splatMap4,  scaledUV).rgb;
	vec3 t5  = texture2D(splatMap5,  scaledUV).rgb;
	vec3 t6  = texture2D(splatMap6,  scaledUV).rgb;
	vec3 t7  = texture2D(splatMap7,  scaledUV).rgb;
	vec3 t8  = texture2D(splatMap8,  scaledUV).rgb;
	vec3 t9  = texture2D(splatMap9,  scaledUV).rgb;
	vec3 t10 = texture2D(splatMap10, scaledUV).rgb;
	vec3 t11 = texture2D(splatMap11, scaledUV).rgb;

    
	// Applying splats from cov maps
	vec3 mixedColor = mix(t0        , t1,  cov0.r);
	mixedColor      = mix(mixedColor, t2,  cov0.g);
	mixedColor      = mix(mixedColor, t3,  cov0.b);
	mixedColor      = mix(mixedColor, t4,  cov0.a);
	mixedColor      = mix(mixedColor, t5,  cov1.r);
	mixedColor      = mix(mixedColor, t5,  cov1.g);
	mixedColor      = mix(mixedColor, t7,  cov1.b);
	mixedColor      = mix(mixedColor, t8,  cov1.a);
	mixedColor      = mix(mixedColor, t9,  cov2.r);
	mixedColor      = mix(mixedColor, t10, cov2.g);
	mixedColor      = mix(mixedColor, t9,  cov2.b);

	// light calculations 
	vec3 normal = texture2D(splatMap6, oUV0.xy).rgb;
	//vec3 lightDir = lightPosition.xyz - (pos.xyz * lightPosition.w);
	//lightDir = normalize(lightDir); 
	//
	//float dist = length(lightDir);
	//float lumination = 1 / (attenuation.y + attenuation.z * dist + attenuation.w * dist * dist);
	//
	//lumination = min(lumination, 1.0);
	//
	//vec3 ha = normalize(lightDir);
	//
	//float NdotL = dot(lightDir, normal);
	//float NdotH = dot(ha, normal);
	//vec4 lit = Lit(NdotL, NdotH, exponent);
	//
	//// return
	//vec4 color = vec4(mixedColor.xyz, 1.0);
	//color *= lumination * (lightDiffuse * lit.y + lightSpecular * lit.z * lit.y);


	// ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightDiffuse.xyz;
  	
    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosition.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightDiffuse.xyz;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(eyePosition - pos.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightDiffuse.xyz;  
        
    vec3 result = (ambient + diffuse + specular) * mixedColor;

    gl_FragColor = vec4(result, 1.0);


    //gl_FragColor = color;
}