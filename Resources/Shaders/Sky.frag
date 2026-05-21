// Shader Code origin from here: https://www.patreon.com/posts/making-stylized-27402644 by MinionsArt
#version 460

layout (set = 2, binding = 0) uniform MaterialUniform {

    // Horizon
    float offset_horizon; // : hint_range(-1.0, 1.0, 0.1) = 0.0;
    float horizon_intensity; // : hint_range(-10.0, 5.0, 0.001) = -3.3;
    vec4 sun_set; // : hint_color = vec4(1.0, 0.8, 1.0, 1.0);
    vec4 horizon_color_day; // : hint_color = vec4(0.0, 0.8, 1.0, 1.0);
    vec4 horizon_color_night; // : hint_color = vec4(0.0, 0.8, 1.0, 1.0);

    // Sun
    vec4 sun_color; // : hint_color = vec4(1.0);
	vec3 sunDirection;
	vec3 eyeDirection;
    float sun_radius; // : hint_range(0.0, 2.0, 0.001) = 0.5;
    bool flat_sun; // = true;

    // Moon
    vec4 moon_color; // : hint_color = vec4(1.0);
    float moon_radius; // : hint_range(0.0, 2.0, 0.01) = 0.15;
    // negative: crescent on right side
    // positive: crescent on left side 
    float moon_crescent; // : hint_range(-0.3, 0.3, 0.001) = -0.3;
    float dark_falloff; // : hint_range(1.0, 6.0, 0.001) = 4.0;

    // Day Background Colors
    vec4 day_bottom_color; // : hint_color = vec4(0.4, 1.0, 1.0, 1.0);
    vec4 day_top_color; // : hint_color = vec4(0.0, 0.8, 1.0, 1.0);

    // Night Background Colors
    vec4 night_bottom_color; // : hint_color = vec4(0.0, 0.0, 0.2, 1.0);
    vec4 night_top_color; // : hint_color = vec4(0.0, 0.0, 0.0, 1.0);

    // stars
    float base_noise_scale; // : hint_range(0.0, 1.0, 0.001) = 0.2;
    float stars_speed; // : hint_range(0.0, 1.0, 0.001) = 0.3;
    float stars_cutoff; // : hint_range(0.0, 1.0, 0.001) = 0.08;
    vec4 stars_sky_color; // : hint_color = vec4(0.0, 0.2, 0.1, 1);

} material;

layout(set = 2, binding = 1) uniform sampler2D stars_texture; // : hint_black;
layout(set = 2, binding = 2) uniform sampler2D base_noise; // : hint_black;

layout(location = 0) in float inTime;
layout(location = 1) in vec3 inEyeDirection;

layout(location = 0) out vec4 outColor;

void main() {

	// get skyUV to place the sun and the moon
	vec2 skyUV = inEyeDirection.xz / inEyeDirection.y;
	
	float base_n = texture(base_noise, (skyUV - inTime) * material.base_noise_scale).x;
	
	// get the middle -> abs of the inEyeDirection to get the horizon
	float horizon = abs((inEyeDirection.y * material.horizon_intensity) - material.offset_horizon);
	vec3 horizonGlow = clamp((1.0 - horizon * 5.0) * clamp(material.sunDirection.y * 10.0, 0.0, 1.0), 0.0, 1.0) * material.horizon_color_day.rgb;// 
	vec3 horizonGlowNight = clamp((1.0 - horizon * m aterial.dark_falloff) * clamp( - material.sunDirection.y * 10.0, 0.0, 1.0), 0.0, 1.0) * material.horizon_color_night.rgb;//
	horizonGlow += horizonGlowNight;
	
	// horizon glow / sunset/ -rise
	float sunset = clamp((1.0 - horizon) * clamp(material.sunDirection.y * 5.0, 0.0, 1.0), 0.0, 1.0);
	vec3 sunsetColoured = sunset * material.sun_set.rgb;
	
	// sun creation
	float sun = distance(inEyeDirection.xyz, material.sunDirection);
	float sunDisc = 1.0 - clamp(sun / material.sun_radius, 0.0, 1.0);
	
	// option to render flat sun
	if (material.flat_sun == true) {
		sunDisc = roundEven(sunDisc);
	}
	
	// moon creation
	float moon = distance(inEyeDirection.xyz, - material.sunDirection);
	float crescentMoon = distance(vec3(inEyeDirection.x + material.moon_crescent, inEyeDirection.yz), - material.sunDirection);
	float crescentMoonDisc = 1.0 - (crescentMoon / material.moon_radius);
	crescentMoonDisc = clamp(crescentMoonDisc * 50.0, 0.0, 1.0);
	float moonDisc = 1.0 - (moon / material.moon_radius);
	moonDisc = clamp(moonDisc * 50.0, 0.0, 1.0);
	moonDisc = clamp(moonDisc - crescentMoonDisc, 0.0, 1.0);
	
	// combine sun and moon
	vec3 sunAndMoon = (sunDisc * material.sun_color.rgb) + (moonDisc * material.moon_color.rgb);
	
	// stars
	vec3 stars = texture(stars_texture, skyUV + (material.stars_speed * (inTime / 20.0))).rgb;
	stars *= clamp(-material.sunDirection.y, 0.0, 1.0);
	stars = step(material.stars_cutoff, stars);
	stars += material.stars_sky_color.rgb;
	//stars += (base_n * stars_sky_color.rgb);
	
	//Sky Background Gradient
	// day color gradient
	vec3 gradientDay = mix(material.day_bottom_color.rgb, material.day_top_color.rgb, clamp(inEyeDirection.y, 0.0, 1.0));
	// night color gradient
	vec3 gradientNight = mix(material.night_bottom_color.rgb, material.night_top_color.rgb, clamp(inEyeDirection.y, 0.0, 1.0));
	vec3 skyGradients = mix(gradientNight, gradientDay, clamp(material.sunDirection.y, 0.0, 1.0));
	
	vec3 sky = skyGradients + sunAndMoon + sunsetColoured + stars + horizonGlow;
	
	outColor = vec4(sky, 1.0);
}