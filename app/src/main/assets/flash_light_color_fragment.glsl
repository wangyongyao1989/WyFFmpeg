#version 320 es
precision mediump float;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;     //聚光的位置向量
    float cutOff;       //一个切光角
    float outerCutOff;  //

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

     float constant;
     float linear;
     float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
        vec3 lightDir = normalize(light.position - FragPos);

       // 检查聚光灯锥体内是否有照明
       float theta = dot(lightDir, normalize(-light.direction));

       //我们使用的角度是余弦而不是度数，因此使用了“>”。
       if(theta > light.cutOff)
       {
           // ambient
           vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

           // diffuse
           vec3 norm = normalize(Normal);
           float diff = max(dot(norm, lightDir), 0.0);
           vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

           // specular
           vec3 viewDir = normalize(viewPos - FragPos);
           vec3 reflectDir = reflect(-lightDir, norm);
           float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
           vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

           // attenuation
           float distance    = length(light.position - FragPos);
           float attenuation = 1.0 / (light.constant + light.linear * distance
                                    + light.quadratic * (distance * distance));
           //消除环境光的衰减，否则在很远的地方，由于else分支中的环境项，聚光灯内部的光线会比外部的光线暗
           // ambient  *= attenuation;
           diffuse *= attenuation;
           specular *= attenuation;

           vec3 result = ambient + diffuse + specular;
           FragColor = vec4(result, 1.0);
       }
       else
       {
           // 否则，使用环境光，这样聚光灯外的场景就不会完全黑暗。
           FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);
       }

}