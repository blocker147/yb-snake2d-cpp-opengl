#version 330 core

in float f_index;
in vec3 f_color;
in vec2 f_textureCoords;

out vec4 FragColor;

// TODO: rewrite on different type of data transportation between CPU & GPU
// 20x20 max world size expected
uniform float u_world_size[400];
uniform float u_world_rotations[400];

uniform sampler2D u_texture_apple;
uniform sampler2D u_texture_snake_head;
uniform sampler2D u_texture_snake_body_tail;
uniform sampler2D u_texture_snake_body_straight;
uniform sampler2D u_texture_snake_body_rotated;
uniform sampler2D u_texture_snake_corpse;
uniform sampler2D u_texture_snake_bone;
uniform sampler2D u_texture_apple_line_spawner;

vec2 rotateTexture(int rotation) {
    switch(rotation) {
        // DEGREES_90
        case 1: return vec2(1.0 - f_textureCoords.y, f_textureCoords.x);
        // DEGREES_180
        case 2: return vec2(1.0 - f_textureCoords.x, 1.0 - f_textureCoords.y);
        // DEGREES_270
        case 3: return vec2(f_textureCoords.y, 1.0 - f_textureCoords.x);
        // DEGREES_0
        case 0:
        default: return f_textureCoords;
    }
}

void main() {
    int index       = int(f_index);
    int gameObject  = int(u_world_size[index]);
    int rotation    = int(u_world_rotations[index]);

    vec2 rotatedTextureCoords = rotateTexture(rotation);

    switch(gameObject) {
        // None (empty cell or simply ground)
        case 0: {
            FragColor = vec4(f_color, 1.0);
            break;
        }
        // Snake Corpse
        case 1: {
            vec4 texColor = texture(u_texture_snake_corpse, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Snake head
        case 2: {
            vec4 texColor = texture(u_texture_snake_head, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Snake Body Straight
        case 3: {
            vec4 texColor = texture(u_texture_snake_body_straight, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Apple
        case 4: {
            vec4 texColor = texture(u_texture_apple, f_textureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Apple Line Spawner
        case 5: {
            vec4 texColor = texture(u_texture_apple_line_spawner, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Snake Bone
        case 6: {
            vec4 texColor = texture(u_texture_snake_bone, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Snake Tail
        case 7: {
            vec4 texColor = texture(u_texture_snake_body_tail, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Snake Body Rotated
        case 8: {
            vec4 texColor = texture(u_texture_snake_body_rotated, rotatedTextureCoords);
            vec4 backgroundColor = vec4(f_color, 1.0);
            FragColor = mix(backgroundColor, texColor, texColor.a);
            break;
        }
        // Pink color in case someone forgot to provide value for 'u_game_object'
        default: {
            FragColor = vec4(1, 0.031, 0.867, 1.0);
            break;
        }
    }
}