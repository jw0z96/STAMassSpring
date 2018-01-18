#version 430
// ----------------------------------------------------------------------------
/// ALL OF THE CONTENT RELATING TO SSAO IS CITED FROM MY
/// REALTIME RENDERING ASSIGNMENT SUBMISSION.
/// SSAO TECHNIQUE IMPLEMENTED:
/// https://learnopengl.com/#!Advanced-Lighting/SSAO
/// PBR TECHNIQUES IMPLEMENTED:
/// https://learnopengl.com/#!PBR/Lighting
// ----------------------------------------------------------------------------

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
uniform vec3 camPos;

uniform vec3 samples[64]; //array of kernel samples being passed in
uniform int SAMPLES = 32; //parameter for number of samples being used
const int MAX_SAMPLES = 64; //parameter for number of samples being passed
const float radius = 0.5; //uniform for SSAO radius
const float bias = 0.1; //uniform for SSAO bias
const float intensity = 2.0; //uniform for SSAO intensity
vec2 noiseScale = windowSize/4; //ensuring the noise is 1px big

const vec3 lightPosition = vec3(5.0, 5.0, 5.0);
const vec3 lightColor = vec3(100.0);
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------

float calcSSAO(vec2 texpos, vec3 normal, vec3 position)
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

// ----------------------------------------------------------------------------
/// PBR FUNCTIONS
// ----------------------------------------------------------------------------

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// ----------------------------------------------------------------------------

vec3 calculateSpecular(vec3 albedo, vec3 normal, vec3 position, float metallic, float roughness, float ao)
{
	// SOURCE FROM https://learnopengl.com/#!PBR/Lighting
	vec3 N = normalize(normal);
	vec3 V = normalize(camPos - position);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// calculate per-light radiance
	vec3 L = normalize(lightPosition - position);
	vec3 H = normalize(V + L);
	float distance    = length(lightPosition - position);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance     = lightColor * attenuation;

	// cook-torrance brdf
	float NDF = DistributionGGX(N, H, roughness);
	float G   = GeometrySmith(N, V, L, roughness);
	vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 nominator    = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular     = nominator / max(denominator, 0.001);

	// add to outgoing radiance Lo
	float NdotL = max(dot(N, L), 0.0);
	vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	vec3 ambient = vec3(0.1) * albedo;
	return (ambient + Lo) * ao;
}

// ----------------------------------------------------------------------------

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

	// early escape for shading lines
	if (WSNormal == vec3(0.0))
	{
		fragColor = vec4(albedo, 1.0);
		return;
	}

	// calculate SSAO
	float SSAO = calcSSAO(texpos, WSNormal, WSPos);
	vec3 fragShaded = calculateSpecular(albedo, WSNormal, WSPos, metalness, roughness, SSAO);

	// HDR tonemapping
	fragShaded = fragShaded / (fragShaded + vec3(1.0));
	// gamma correct
	fragShaded = pow(fragShaded, vec3(1.0/2.2));

	fragColor = vec4(fragShaded, 1.0);
}
