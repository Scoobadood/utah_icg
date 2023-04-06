#version 410 core

layout (location=0) out vec4 frag_colour;

uniform float amb_light;

uniform vec3  diff_light_dir;
uniform float diff_light_int;

uniform vec3  spec_light_colour;
uniform float spec_light_alpha;

in vec3 frag_normal;
in vec3 frag_position;

void main() {
    vec3 nn            = normalize(frag_normal);

    // Calc diffuse lighting geometry component
    float n_dot_w      = max(0,dot(diff_light_dir, nn));

    // Calc specular lighting Phong
    vec3 look          = -normalize(frag_position);
    vec3 r             = -reflect(diff_light_dir, nn);
    float r_dot_v      = max(0,dot(r, look));
    float spec_coeff   = pow(r_dot_v, spec_light_alpha);

    vec3 spec_light    = diff_light_int * spec_light_colour * spec_coeff;
    spec_light         = max(vec3(0,0,0),spec_light);
    vec3 ambient_light = amb_light * vec3(1,0,0);
    vec3 diff_light    = diff_light_int * n_dot_w * vec3(1,0,0);

    frag_colour        = vec4(ambient_light + diff_light + spec_light, 1);
}