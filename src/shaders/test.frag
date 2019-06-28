#version 450 core

layout(location = 0) in vec3 frag_position;
layout(location = 1) in vec2 frag_texcoord;
layout(location = 2) in vec3 frag_normal;

uniform vec4 base_color_factor;
uniform bool has_color_map;
uniform sampler2D color_map;

uniform vec2 metallic_roughness_values;
uniform float occlusion_strength;
uniform bool has_metallic_roughness_map;
uniform bool has_occlusion_map;
uniform sampler2D metallic_roughness_map;

uniform bool has_normal_map;
uniform sampler2D normal_map;

uniform vec3 emissive_factor;
uniform bool has_emissive_map;
uniform sampler2D emissive_map;

uniform vec3 cam_pos;
uniform vec3 light_directions[3];
uniform vec3 light_colors[3];

uniform bool only_color_map;

out vec4 out_color;

// https://github.com/KhronosGroup/glTF-WebGL-PBR
struct PBRInfo
{
    float NdotL;               // cos angle between normal and light direction
    float NdotV;               // cos angle between normal and view direction
    float NdotH;               // cos angle between normal and half vector
    float LdotH;               // cos angle between light direction and half vector
    float VdotH;               // cos angle between view direction and half vector
    float perceptualRoughness; // roughness value, as authored by the model
                               // creator (input to shader)
    float metalness;           // metallic value at the surface
    vec3 reflectance0;         // full reflectance color (normal incidence angle)
    vec3 reflectance90;        // reflectance color at grazing angle
    float alphaRoughness;      // roughness mapped to a more linear change in the
                               // roughness (proposed by [2])
    vec3 diffuseColor;         // color contribution from diffuse lighting
    vec3 specularColor;        // color contribution from specular lighting
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal()
{
    // Retrieve the tangent space matrix
    vec3 pos_dx = dFdx(frag_position);
    vec3 pos_dy = dFdy(frag_position);
    vec3 tex_dx = dFdx(vec3(frag_texcoord, 0.0));
    vec3 tex_dy = dFdy(vec3(frag_texcoord, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
    vec3 ng = normalize(frag_normal);

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);

    if (has_normal_map)
    {
        vec3 n = texture(normal_map, frag_texcoord).rgb;
        return normalize(tbn * ((2.0 * n - 1.0)));
    }
    else
    {
        // The tbn matrix is linearly interpolated, so we need to re-normalize
        return normalize(tbn[2].xyz);
    }
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria
// https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs) { return pbrInputs.diffuseColor / M_PI; }

// The following equation models the Fresnel reflectance term of the spec
// equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 specularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 +
           (pbrInputs.reflectance90 - pbrInputs.reflectance0) *
               pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their
// modifications to
// alphaRoughness as input as originally proposed in [2].
float geometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL =
        2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV =
        2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across
// the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened
// Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course
// notes from EPIC Games [1], Equation 3.
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f =
        (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
    return roughnessSq / (M_PI * f * f);
}

void main()
{

    out_color = vec4(frag_normal, 1.);
    return;


}
