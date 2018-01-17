#version 430

// our input textures from our framebuffer object
layout(binding = 0) uniform sampler2D WSPositionTex;
layout(binding = 1) uniform sampler2D WSNormalTex;
layout(binding = 2) uniform sampler2D depthTex;
layout(binding = 3) uniform sampler2D albedoTex;
layout(binding = 4) uniform sampler2D metalRoughTex;
layout(binding = 5) uniform sampler2D noiseTex;

// The output colour. At location 0 it will be sent to the screen.
layout (location = 0) out vec4 fragColor;

// We pass the window size to the shader.
uniform vec2 windowSize;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 camPos;

uniform vec3 samples[64]; //array of kernel samples being passed in
uniform int SAMPLES = 64; //parameter for number of samples being used
const int MAX_SAMPLES = 64; //parameter for number of samples being passed

const float radius = 0.5; //uniform for SSAO radius
const float bias = 0.25; //uniform for SSAO bias
const float intensity = 2; //uniform for SSAO intensity

vec2 noiseScale = windowSize/4; //ensuring the noise is 1px big

float  calcSSAO(vec2 texpos, vec3 normal, vec3 position)
{
	//SOURCE FROM https://learnopengl.com/#!Advanced-Lighting/SSAO BEGINS HERE
	vec3 randomVec = texture(noiseTex, texpos * noiseScale).rgb; //this should be random
	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);
	float origDepth = (MVP * vec4(position, 1.0)).z;
	float occlusion = 0.0;
	for (int i = 0; i < MAX_SAMPLES; i+=MAX_SAMPLES/SAMPLES)
	{
		// get sample position:
		vec3 SSAOsample = TBN * samples[i];
		SSAOsample = position + SSAOsample * radius;
		vec4 offset = MVP * vec4(SSAOsample, 1.0);
		offset /= offset.w;
		vec2 offsetCoords = offset.xy * 0.5 + vec2(0.5);
		// get sample depth:
		float sampleDepth = (MVP * vec4(texture(WSPositionTex, offsetCoords).xyz, 1.0)).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(origDepth - sampleDepth));
		occlusion += (origDepth >= sampleDepth + bias ? 1.0 : 0.0) * rangeCheck;
	}
	//SOURCE FROM https://learnopengl.com/#!Advanced-Lighting/SSAO ENDS HERE
	return 1.0 - intensity * (occlusion / SAMPLES);
}


void main()
{
	// Determine the texture coordinate from the window size
	vec2 texpos = gl_FragCoord.xy / windowSize;
	// Access the world space position texture
	vec3 WSPos = texture(WSPositionTex, texpos).rgb;
	// Access the world space normal texture
	vec3 WSNormal = texture(WSNormalTex, texpos).rgb;
	// Access the screen space depth texture
	float depth = texture(depthTex, texpos).r;
	// Access the albedo texture
	vec3 albedo = texture(albedoTex, texpos).rgb;
	// Access the metallic & roughness texture
	float metalness = texture(metalRoughTex, texpos).r;
	float roughness = texture(metalRoughTex, texpos).g;

	// calculate SSAO
	float SSAO = calcSSAO(texpos, WSNormal, WSPos);
	vec3 fragShaded = albedo * SSAO;
	fragColor = vec4(fragShaded, 1.0);
}
