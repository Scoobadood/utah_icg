#version 410 core

layout (location=0) out vec4 frag_colour;


uniform float ks;       // Specular constant
uniform float kd;       // Diffuse constant
uniform float ka;       // Ambient constant
uniform float alpha;    // Shininess
uniform vec3  colour;

uniform vec3 light_dir;// From light. Invert for dir to light
uniform float light_int;// Light intensity.

uniform vec3 spot_light_pos[8]; // From light. Invert for dir to light
uniform vec3 spot_light_dir[8]; // Facing of main axis of light
uniform float spot_light_int[8];// Light intensity.
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

    // Now do the spotlights
    vec3 spot_spec_light = vec3(0);
    vec3 spot_diff_light = vec3(0);

    for ( int i=0; i<8; ++i ) {
        n_ld            = normalize(spot_light_pos[i] - frag_position);
        n_dot_w         = max(0, dot(n_ld, n_n));

        float beam_pct = 1.0f;

        // Determine how much of beam falls on object
        vec3 n_sd      = -normalize(spot_light_dir[i]);
        float angle    = acos(dot(n_ld, n_sd));
        beam_pct       = smoothstep(0, spot_light_angle, (spot_light_angle - angle));
        beam_pct       = beam_pct * beam_pct;

        // Calc specular lighting Phong
        r                  = -reflect(n_ld, n_n);
        r_dot_v            = max(0, dot(r, look));
        spec_coeff         = pow(r_dot_v, alpha);

        spot_spec_light = spot_spec_light + (spot_light_int[i] * ks * spec_coeff * vec3(1,1,1) * beam_pct); //spot_spec_light + ks * spec_coeff * spot_light_int[i] * out_beam  * spot_light_colour;
        spot_diff_light = spot_diff_light + (colour * spot_light_int[i] * kd * n_dot_w * beam_pct);
    }

    vec3 l2 = spot_spec_light + spot_diff_light;
    frag_colour        = vec4(l1+l2, 1);
}