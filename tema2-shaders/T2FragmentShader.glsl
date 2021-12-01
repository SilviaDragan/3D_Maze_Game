#version 330

// Input
in vec3 frag_color;

// TODO(student): Get values from vertex shader
layout(location = 0) out vec3 v_position;
layout(location = 1) out vec3 v_color;

// Output
out vec4 out_color;

void main()
{
    // TODO(student): Write pixel out color
    out_color = vec4(frag_color, 1);

}
