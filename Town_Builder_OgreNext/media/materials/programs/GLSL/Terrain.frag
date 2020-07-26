#version 120

uniform sampler2D covMap1;
uniform sampler2D splatMap1;
uniform sampler2D splatMap2;
uniform sampler2D splatMap3;

varying vec2 oUV0;
varying vec4 pos;

void main ()
{

	vec3 cov1 = texture2D(covMap1, oUV0).rgb;
	
	vec2 scaledUV = oUV0 * 16;
	
	// calculate Splatting
	//gl_FragColor =  texture2D(splatMap1, oUV0.yx) * cov1.x
    //              + texture2D(splatMap2, oUV0.yx) * cov1.y
    //              + texture2D(splatMap3, oUV0.yx) * cov1.z;
				  
				  
	vec3 t1 = texture2D(splatMap1, scaledUV).rgb;
	vec3 t2 = texture2D(splatMap2, scaledUV).rgb;
	vec3 t3 = texture2D(splatMap3, scaledUV).rgb;

    
	vec3 mixedColor = mix(t1, t2, cov1.x);
	mixedColor = mix(mixedColor, t3, cov1.y);
	
    gl_FragColor = vec4(mixedColor.xyz, 1.0);

}