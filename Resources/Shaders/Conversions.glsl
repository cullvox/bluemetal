
// Converts a linear RGB color (range [0.0, 1.0]) to Display P3 gamma space
vec4 LinearToDisplayP3(vec4 linearRGB) {
    vec4 nonlinearRGB;

    for (int i = 0; i < 3; i++) {
        if (linearRGB[i] <= 0.0031308) {
            nonlinearRGB[i] = 12.92 * linearRGB[i];
        } else {
            nonlinearRGB[i] = 1.055 * pow(linearRGB[i], 1.0 / 2.4) - 0.055;
        }
    }

    return nonlinearRGB;
}

// Converts a color from sRGB gamma to linear light gamma
vec4 LinearToSRGB(vec4 linearRGB)
{
    bvec3 cutoff = lessThan(linearRGB.rgb, vec3(0.0031308));
    vec3 higher = vec3(1.055)*pow(linearRGB.rgb, vec3(1.0/2.4)) - vec3(0.055);
    vec3 lower = linearRGB.rgb * vec3(12.92);

    return vec4(mix(higher, lower, cutoff), linearRGB.a);
}

const uint VIEWPORT_COLOR_SPACE_NOOP = 0;
const uint VIEWPORT_COLOR_SPACE_SRGB = 1;
const uint VIEWPORT_COLOR_SPACE_P3 = 2;

vec4 ConvertColorSpace(uint colorSpace, vec4 color)
{

    switch (colorSpace)
    {
    case VIEWPORT_COLOR_SPACE_NOOP:
        return color;
    case VIEWPORT_COLOR_SPACE_SRGB:
        return LinearToSRGB(color);
    case VIEWPORT_COLOR_SPACE_P3:
        return LinearToDisplayP3(color);
    default:
        return color;
    }

}