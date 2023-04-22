#version 430
#extension GL_NV_shadow_samplers_cube : enable

uniform vec4 lightPos;
uniform vec4 lightColor;

uniform vec3 camPos;

uniform vec4 waterColor;
uniform vec4 landColor;
uniform vec4 mountainColor;

uniform float waterLevel;
uniform float landLevel;
uniform float mountainLevel;





in vec4 fpos;
in vec4 fnorm;
flat in int fTextType;
in vec4 fcolor;
flat in float fnoise;

out vec4 gli_FragColor;

void main()
{	vec4 testColor = fcolor;

        if(fnoise < waterLevel)
		{
			testColor = fcolor * waterColor;
        }
        else if(fnoise < landLevel)
        {
            testColor = fcolor * landColor;
		}
		else if(fnoise < mountainLevel)
		{
			testColor = fcolor * mountainColor;
		}
		else
		{
			testColor = fcolor;
		}

		float ambient = 0.20f;
		
        vec4 normal = normalize(fnorm);
        vec4 lightDir = normalize(lightPos - fpos);        
        float diffuse = max(dot(normal, lightDir), 0.0);

		float specularLight = 0.5;
		vec3 viewDir = normalize(camPos - fpos.xyz);
		vec4 reflectDir = reflect(-lightDir, normal);
		float specAmount = pow(max(dot(viewDir, reflectDir.xyz), 0.0), 8);
		float specular = specularLight * specAmount;
		
		//specular no funciona
        //gli_FragColor = testColor * lightColor * (diffuse+ambient+specular);
		//gli_FragColor = testColor * lightColor * (diffuse+ambient);
        
        gli_FragColor = testColor;
}