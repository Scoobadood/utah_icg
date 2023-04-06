#version 410 core

layout (location=0) out vec4 frag_colour;


uniform float ks;       // Specular constant
uniform float kd;       // Diffuse constant
uniform float ka;       // Ambient constant
uniform float alpha;    // Shininess
uniform vec3  colour;

uniform vec3 light_dir;// From light. Invert for dir to light
uniform float light_int;// Light intensity.

uniform vec3 spot_light_pos; // From light. Invert for dir to light
uniform vec3 spot_light_dir; // Facing of main axis of light
uniform float spot_light_int;// Light intensity.
uniform vec3 spot_light_colour;// Light intensity.
uniform float spot_light_angle; // half beam angle

in vec3 frag_normal;
in vec3 frag_position;

void main() {
    vec3 n_n           = normalize(frag_normal);
    vec3 n_ld          = normalize(-light_dir);

    // Calc diffuse lighting geometry component
    float n_dot_w      = max(0, dot(n_ld, n_n));

    // Calc specular lighting Phong
    vec3 look          = -normalize(frag_position);
    vec3 r             = -reflect(n_ld, n_n);
    float r_dot_v      = max(0, dot(r, look));
    float spec_coeff   = pow(r_dot_v, alpha);

    vec3 spec_light    = ks * spec_coeff * light_int * vec3(1,1,1);
    vec3 ambient_light = ka * light_int * colour;
    vec3 diff_light    = kd * n_dot_w * light_int * colour;
    vec3 l1 = diff_light + spec_light;

    // Now repeat for the spotlight
    // Light direction is computed from fragment to light
    n_ld          = normalize(spot_light_pos - frag_position);

    // If this direction is outside of the cone of light then we have no contribution from it.
    vec3 n_sd          = normalize(-spot_light_dir);
    float p = dot(n_ld, n_sd);
    float out_beam = 0;
    if( p > spot_light_angle ) out_beam = smoothstep(spot_light_angle, 1,p);
    out_beam = out_beam * out_beam;

    // Calc diffuse lighting geometry component
    n_dot_w      = max(0, dot(n_ld, n_n));

    // Calc specular lighting Phong
    r             = -reflect(n_ld, n_n);
    r_dot_v            = max(0, dot(r, look));
    spec_coeff         = pow(r_dot_v, alpha);

    vec3 spot_spec_light    = ks * spec_coeff * spot_light_int * out_beam  * vec3(1,1,1);
    vec3 spot_diff_light    = kd * n_dot_w * spot_light_int * out_beam * colour;

    vec3 l2 = spot_spec_light + spot_diff_light;

    frag_colour        = vec4(l1+l2+ambient_light,1);
//    frag_colour        = vec4(l1+l2, 1);
}