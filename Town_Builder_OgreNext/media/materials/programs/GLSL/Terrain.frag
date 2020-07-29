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

varying vec2 oUV0;

void main ()
{
	// Scaling UV map to get tiling effect
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
	mixedColor      = mix(mixedColor, t6,  cov1.g);
	mixedColor      = mix(mixedColor, t7,  cov1.b);
	mixedColor      = mix(mixedColor, t8,  cov1.a);
	mixedColor      = mix(mixedColor, t9,  cov2.r);
	mixedColor      = mix(mixedColor, t10, cov2.g);
	mixedColor      = mix(mixedColor, t9,  cov2.b);
	//mixedColor      = mix(mixedColor, t0,  cov2.a);

	// return
    gl_FragColor = vec4(mixedColor.xyz, 1.0);
}