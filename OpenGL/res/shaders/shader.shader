#shader vertex
#version 330 core

layout(location = 0) in vec4 position;


uniform mat4 u_MVP;

void main(){
	gl_Position = u_MVP * position;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;


uniform vec4 u_Color;

float near = 0.1;
float far = 10.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(){
	//color = u_Color;
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	color = vec4(vec3(depth), 1.0) * u_Color;
};