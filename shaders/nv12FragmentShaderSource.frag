#version 330
precision mediump float;
in vec2 v_texcoord;
uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureUV;
out vec4 fragColor;

void main() {
    mediump float y;
    mediump vec2 uv;
    y =    texture  (s_textureY, v_texcoord).r;
    uv =    texture  (s_textureUV, v_texcoord).ra -  vec2(0.5, 0.5);
    fragColor = vec4(y + 1.403 * uv.y,
                       y - 0.344 * uv.x - 0.714 * uv.y,
                       y + 1.770 * uv.x,
                       1.0);
} ;
