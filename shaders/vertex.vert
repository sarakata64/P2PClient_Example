#version  330

in vec2 position;
in vec2 texcoord;
out vec2 v_texcoord;

void main(void)
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    v_texcoord = texcoord;
}
