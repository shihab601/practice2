#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
    vec3 position;
    float k_c;
    float k_l;
    float k_q;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

vec3 CalcPointLight(Material m, PointLight light, vec3 N, vec3 fragPos, vec3 V);
vec3 CalcSpotLight(Material m, SpotLight light, vec3 N, vec3 fragPos, vec3 V);

void main()
{
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(material, pointLights[i], N, FragPos, V);

    result += CalcSpotLight(material, spotLight, N, FragPos, V);
    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(Material m, PointLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);

    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * d * d);

    vec3 ambient = m.ambient * light.ambient;
    vec3 diffuse = m.diffuse * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = m.specular * pow(max(dot(V, R), 0.0), m.shininess) * light.specular;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(Material m, SpotLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);

    float theta = dot(L, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = m.ambient * light.ambient;
    vec3 diffuse = m.diffuse * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = m.specular * pow(max(dot(V, R), 0.0), m.shininess) * light.specular;

    return (ambient + diffuse + specular) * intensity;
}
