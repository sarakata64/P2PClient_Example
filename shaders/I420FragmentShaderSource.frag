// Fragment shader converts YUV values from input textures into a final RGB
// pixel. The conversion formula is from http://www.fourcc.org/fccyvrgb.php.

# version 330
precision highp float;

in vec2 v_texcoord;

uniform lowp sampler2D s_textureY;
uniform lowp sampler2D s_textureU;
uniform lowp sampler2D s_textureV;

out vec4 fragColor;

void main() {

   float y, u, v, r, g, b;
    y =  texture (s_textureY, v_texcoord).r;
    u = texture(s_textureU, v_texcoord).r;
    v = texture(s_textureV, v_texcoord).r;
    u = u - 0.5;
    v = v - 0.5;
    r = y + 1.403 * v;
    g = y - 0.344 * u - 0.714 * v;
    b = y + 1.770 * u;

     fragColor = vec4(r, g, b, 1.0);
  };
