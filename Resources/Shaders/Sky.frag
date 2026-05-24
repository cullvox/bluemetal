// Shader Code origin from here: https://www.patreon.com/posts/making-stylized-27402644 by MinionsArt
#version 460

layout (set = 2, binding = 0) uniform MaterialUniform {

    // Horizon
    float offsetHorizon; // : hint_range(-1.0, 1.0, 0.1) = 0.0;
    float horizonIntensity; // : hint_range(-10.0, 5.0, 0.001) = -3.3;
    vec4 cSunSet; // : hint_color = vec4(1.0, 0.8, 1.0, 1.0);
    vec4 cHorizonDay; // : hint_color = vec4(0.0, 0.8, 1.0, 1.0);
    vec4 cHorizonNight; // : hint_color = vec4(0.0, 0.8, 1.0, 1.0);

    // Sun
    vec4 cSun; // : hint_color = vec4(1.0);
	vec3 sunDirection;
	vec3 eyeDirection;
    float sunRadius; // : hint_range(0.0, 2.0, 0.001) = 0.5;
    bool bFlatSun; // = true;

    // Moon
    vec4 cMoon; // : hint_color = vec4(1.0);
    float moonRadius; // : hint_range(0.0, 2.0, 0.01) = 0.15;
    // negative: crescent on right side
    // positive: crescent on left side 
    float moonCrescent; // : hint_range(-0.3, 0.3, 0.001) = -0.3;
    float darkFalloff; // : hint_range(1.0, 6.0, 0.001) = 4.0;

	// Horizon Fade
	bool bEnableHorizonFade;
	float horizonFadeSize;
	float horizonFadeBlend;

    // Day Background Colors
    vec4 cDayBottom; // : hint_color = vec4(0.4, 1.0, 1.0, 1.0);
    vec4 cDayTop; // : hint_color = vec4(0.0, 0.8, 1.0, 1.0);

    // Night Background Colors
    vec4 cNightBottom; // : hint_color = vec4(0.0, 0.0, 0.2, 1.0);
    vec4 cNightTop; // : hint_color = vec4(0.0, 0.0, 0.0, 1.0);

    // stars
    float baseNoiseScale; // : hint_range(0.0, 1.0, 0.001) = 0.2;
    float starsSpeed; // : hint_range(0.0, 1.0, 0.001) = 0.3;
    float starsCutoff; // : hint_range(0.0, 1.0, 0.001) = 0.08;
    vec4 cStarsSky; // : hint_color = vec4(0.0, 0.2, 0.1, 1);
	float offsetStars;
	float starsIntensity;
	float starFalloff;
	float starsFadeModulation;

} material;

layout(set = 2, binding = 1) uniform sampler2D starsTexture; // : hint_black;
layout(set = 2, binding = 2) uniform sampler2D baseNoiseTexture; // : hint_black;

layout(location = 0) in float inTime;
layout(location = 1) in vec3 inEyeDir;
layout(location = 2) in vec2 inViewportSize;
layout(location = 3) in mat4 inInverseProjection;

layout(location = 0) out vec4 outColor;

void main() {

	vec3 eyeDir = normalize(inEyeDir);
	vec3 sunDir = -normalize(material.sunDirection);

	// get skyUV to place the sun and the moon
	vec2 skyUV = eyeDir.xz / eyeDir.y;
	
	float base_n = texture(baseNoiseTexture, (skyUV - inTime) * material.baseNoiseScale).x;
	
	// get the middle -> abs of the eyeDir to get the horizon
	float horizon = abs((eyeDir.y * material.horizonIntensity) - material.offsetHorizon);
	vec3 horizonGlow = clamp((1.0 - horizon * 5.0) * clamp(sunDir.y * 10.0, 0.0, 1.0), 0.0, 1.0) * material.cHorizonDay.rgb;// 
	vec3 horizonGlowNight = clamp((1.0 - horizon * material.darkFalloff) * clamp( - sunDir.y * 10.0, 0.0, 1.0), 0.0, 1.0) * material.cHorizonNight.rgb;//
	horizonGlow += horizonGlowNight;
	
	// horizon glow / sunset/ -rise
	float sunset = clamp((1.0 - horizon) * clamp(sunDir.y * 5.0, 0.0, 1.0), 0.0, 1.0);
	vec3 sunsetColoured = sunset * material.cSunSet.rgb;
	
	// sun creation
	float sun = distance(eyeDir.xyz, sunDir);
	float sunDisc = 1.0 - clamp(sun / material.sunRadius, 0.0, 1.0);
	
	// option to render flat sun
	if (material.bFlatSun == true) {
		sunDisc = roundEven(sunDisc);
	}
	
	
	// moon creation
	float moon = distance(eyeDir.xyz, - sunDir);
	float crescentMoon = distance(vec3(eyeDir.x + material.moonCrescent, eyeDir.yz), - sunDir);
	float crescentMoonDisc = 1.0 - (crescentMoon / material.moonRadius);
	crescentMoonDisc = clamp(crescentMoonDisc * 50.0, 0.0, 1.0);
	float moonDisc = 1.0 - (moon / material.moonRadius);
	moonDisc = clamp(moonDisc * 50.0, 0.0, 1.0);
	moonDisc = clamp(moonDisc - crescentMoonDisc, 0.0, 1.0);
	
	// combine sun and moon
	float topMask = step(0.0, eyeDir.y) * horizon;
	topMask = smoothstep(material.horizonFadeSize, material.horizonFadeSize + material.horizonFadeBlend, topMask);
	topMask = mix(1.0, topMask, float(material.bEnableHorizonFade));
	vec3 sunAndMoon = (sunDisc * material.cSun.rgb) + (moonDisc * material.cMoon.rgb);
	sunAndMoon *= topMask;

	// stars
	float starsParameters = abs((eyeDir.y * material.starsIntensity) - material.offsetStars) * step(0.0, eyeDir.y) * material.starFalloff;
	vec3 stars = texture(starsTexture, (skyUV * starsParameters) + (material.starsSpeed * (inTime / 30.0))).rgb;
	float starsZenith = clamp(-material.sunDirection.y, 0.0, 1.0);
	stars *= starsZenith + abs(starsZenith - material.starsFadeModulation);
	stars = step(material.starsCutoff, stars);
	stars += material.cStarsSky.rgb;
	
	//Sky Background Gradient
	// day color gradient
	vec3 gradientDay = mix(material.cDayBottom.rgb, material.cDayTop.rgb, clamp(eyeDir.y, 0.0, 1.0));
	// night color gradient
	vec3 gradientNight = mix(material.cNightBottom.rgb, material.cNightTop.rgb, clamp(eyeDir.y, 0.0, 1.0));
	vec3 skyGradients = mix(gradientNight, gradientDay, clamp(sunDir.y, 0.0, 1.0));
	
	vec3 sky = skyGradients + sunAndMoon + sunsetColoured + stars + horizonGlow;
	
	outColor = vec4(sky, 1.0);
}