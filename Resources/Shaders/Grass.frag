

void fragment() {
    AO = bottom_to_top - currentWindBend * windAOEffect;
    AO_LIGHT_AFFECT = 1.0;
    ALBEDO = mix(colorSmall, colorLarge, patchFactor);
    BACKLIGHT = vec3(backLightColor);
    ROUGHNESS = roughnessFactor;
    NORMAL = mix(NORMAL, vec3(0.0,1.0,0.0), bottom_to_top);
    //if(!FRONT_FACING) NORMAL = -NORMAL;
    SPECULAR = speculrFactor;
}